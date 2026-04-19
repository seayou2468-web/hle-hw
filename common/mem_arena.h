// Copyright (C) 2003 Dolphin Project.
// Licensed under GPLv2 or later.

#pragma once

#include "common.h"

// iOS-only memory arena implementation.
class MemArena {
public:
    void GrabLowMemSpace(size_t size);
    void ReleaseSpace();
    void* CreateView(s64 offset, size_t size, void* base = nullptr);
    void ReleaseView(void* view, size_t size);

    // iOS uses 64-bit VA space; fixed 4GB base probing is unnecessary.
    static u8* Find4GBBase();

private:
    int fd{-1};
};

enum {
    MV_MIRROR_PREVIOUS = 1,
    MV_IS_PRIMARY_RAM = 0x100,
    MV_IS_EXTRA1_RAM = 0x200,
    MV_IS_EXTRA2_RAM = 0x400,
};

struct MemoryView {
    u8** out_ptr_low;
    u8** out_ptr;
    u32 virtual_address;
    u32 size;
    u32 flags;
};

u8* MemoryMap_Setup(const MemoryView* views, int num_views, u32 flags, MemArena* arena);
void MemoryMap_Shutdown(const MemoryView* views, int num_views, u32 flags, MemArena* arena);
