/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "../kernel.h"

#define ASSERT(CONDITION) \
    if (CONDITION) {} else { panic("Assertion failure"); }