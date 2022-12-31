/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: rtc.h
*/

#pragma once

#include <common.h>

#define RTC_REG_SECOND 0x00
#define RTC_REG_MINUTE 0x02
#define RTC_REG_HOUR   0x04
#define RTC_REG_DAY    0x07
#define RTC_REG_MONTH  0x08
#define RTC_REG_YEAR   0x09

typedef struct {
    u8 century;
    u8 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
} rtc_time_t;

extern rtc_time_t crt_time;

int rtc_updating();

u8 get_rtc_register(int num);
void set_rtc_register(u16 reg, u8 val);

void rtc_read();
void rtc_write(rtc_time_t *rt);
void init_rtc();

void print_time(rtc_time_t *rt);
void print_crt_time();

void rtc_sleep(u32 seconds);