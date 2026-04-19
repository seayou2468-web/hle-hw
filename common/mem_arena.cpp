// Copyright (C) 2003 Dolphin Project.
// Licensed under GPLv2 or later.

#include <cerrno>
#include <cstring>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "memory_util.h"
#include "string_util.h"
#include "timer.h"
#include "mem_arena.h"

namespace {

std::string BuildShmName() {
    return StringFromFormat("/mikage_mem_arena_%d_%llu", static_cast<int>(getpid()),
                            static_cast<unsigned long long>(Common::Timer::GetTimeMs()));
}

} // namespace

void MemArena::GrabLowMemSpace(size_t size) {
    ReleaseSpace();

    const std::string name = BuildShmName();
    fd = shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        PanicAlert("shm_open failed: %s", strerror(errno));
        return;
    }

    shm_unlink(name.c_str());

    if (ftruncate(fd, static_cast<off_t>(size)) < 0) {
        PanicAlert("ftruncate failed: %s", strerror(errno));
        close(fd);
        fd = -1;
    }
}

void MemArena::ReleaseSpace() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void* MemArena::CreateView(s64 offset, size_t size, void* base) {
    if (fd < 0) {
        return nullptr;
    }

    const int mmap_flags = MAP_SHARED | (base ? MAP_FIXED : 0);
    void* ptr = mmap(base, size, PROT_READ | PROT_WRITE, mmap_flags, fd, static_cast<off_t>(offset));
    return ptr == MAP_FAILED ? nullptr : ptr;
}

void MemArena::ReleaseView(void* view, size_t size) {
    if (!view || view == MAP_FAILED) {
        return;
    }
    munmap(view, size);
}

u8* MemArena::Find4GBBase() {
    return nullptr;
}

static bool Memory_TryBase(const MemoryView* views, int num_views, u32 flags, MemArena* arena,
                           u8*& effective_base) {
    std::vector<void*> mapped_views;
    mapped_views.reserve(static_cast<size_t>(num_views));

    for (int i = 0; i < num_views; ++i) {
        const MemoryView& view = views[i];
        const u32 offset = (view.flags & MV_MIRROR_PREVIOUS) ? views[i - 1].virtual_address
                                                              : view.virtual_address;

        void* target = effective_base ? static_cast<void*>(effective_base + view.virtual_address) : nullptr;
        void* mapped = arena->CreateView(offset, view.size, target);
        if (!mapped) {
            for (size_t n = 0; n < mapped_views.size(); ++n) {
                arena->ReleaseView(mapped_views[n], views[static_cast<int>(n)].size);
            }
            return false;
        }

        if (!effective_base) {
            effective_base = reinterpret_cast<u8*>(mapped) - view.virtual_address;
        }

        *view.out_ptr_low = reinterpret_cast<u8*>(mapped);
        *view.out_ptr = *view.out_ptr_low - view.virtual_address;

        if (flags & view.flags) {
            NOTICE_LOG(MEMMAP, "MemoryView[%d]: %08x - %08x", i, view.virtual_address,
                       view.virtual_address + view.size);
        }

        mapped_views.push_back(mapped);
    }

    return true;
}

u8* MemoryMap_Setup(const MemoryView* views, int num_views, u32 flags, MemArena* arena) {
    u32 backing_size = 0;
    for (int i = 0; i < num_views; ++i) {
        if (!(views[i].flags & MV_MIRROR_PREVIOUS)) {
            backing_size = std::max(backing_size, views[i].virtual_address + views[i].size);
        }
    }

    arena->GrabLowMemSpace(backing_size);

    u8* effective_base = nullptr;
    if (!Memory_TryBase(views, num_views, flags, arena, effective_base)) {
        arena->ReleaseSpace();
        PanicAlert("MemoryMap_Setup: failed to map views");
        return nullptr;
    }

    return effective_base;
}

void MemoryMap_Shutdown(const MemoryView* views, int num_views, u32 flags, MemArena* arena) {
    for (int i = 0; i < num_views; ++i) {
        if (views[i].flags & MV_MIRROR_PREVIOUS) {
            continue;
        }
        arena->ReleaseView(*views[i].out_ptr_low, views[i].size);
        *views[i].out_ptr_low = nullptr;
        *views[i].out_ptr = nullptr;

        if (flags & views[i].flags) {
            NOTICE_LOG(MEMMAP, "Shutdown view %d", i);
        }
    }

    arena->ReleaseSpace();
}
