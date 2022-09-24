/*
    Copyright (c) 2022 thatOneArchUser
    All rights reserved
*/

#pragma once

#define DEBUG

static char *history[256];

void execute_command(char *input);
void panic(char *message);