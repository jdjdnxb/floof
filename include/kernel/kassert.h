#pragma once

#include <panic.h>

#define KASSERT(condition) \
    do { \
        if (!(condition)) \
            panic("Assertion failed: " #condition, nullptr); \
    } while (0)
