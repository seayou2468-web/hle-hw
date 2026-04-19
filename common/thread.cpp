// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "thread.h"
#include "common.h"


#include <mach/mach.h>

namespace Common
{

int CurrentThreadId()
{
    return mach_thread_self();
}

void SetThreadAffinity(std::thread::native_handle_type thread, u32 mask)
{
    (void)thread;
    (void)mask;
}

void SetCurrentThreadAffinity(u32 mask)
{
    SetThreadAffinity(pthread_self(), mask);
}

void SleepCurrentThread(int ms)
{
    usleep(1000 * ms);
}

void SwitchCurrentThread()
{
    usleep(1000 * 1);
}

void SetCurrentThreadName(const char* szThreadName)
{
    pthread_setname_np(szThreadName);
}

} // namespace Common
