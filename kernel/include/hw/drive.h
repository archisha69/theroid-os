/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#include "types.h"

#define SECTOR_SIZE 512

void init_drive();
void read_drive(char *buff, u32 start, u32 len);