// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "common.h"

// iOS path: use fallback TLS define for this translation unit.
#define __thread

// Generic function to get last error message.
// Call directly after the command or use the error num.
// This function might change the error code.
const char* GetLastErrorMsg()
{
    static const size_t buff_size = 255;
    static __thread char err_str[buff_size] = {};

    // Thread safe (XSI-compliant)
    strerror_r(errno, err_str, buff_size);

    return err_str;
}
