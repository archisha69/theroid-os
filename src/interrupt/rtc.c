/* 
    Copyright (c) 2022-2023, thatOneArchUser
    All rights reserved.

    File: rtc.c
*/

#include <io.h>
#include <rtc.h>
#include <stdio.h>
#include <kheap.h>
#include <string.h>

rtc_time_t crt_time;

u32 rtc_to_int(rtc_time_t *a) {
    int n = a->second;
    n += a->minute * 60;
    n += a->hour * 3600;
    return n;
}

void rtc_sleep(u32 seconds) {
    rtc_read();
    int start = rtc_to_int(&crt_time);
    while (start + seconds > rtc_to_int(&crt_time)) {
        rtc_read();
    }
}

int rtc_updating() {
    outb(CMOS_CMD, 0x0A);
    u32 status = inb(CMOS_DAT);
    return status & 0x80;
}

u8 get_rtc_register(int num) {
    outb(CMOS_CMD, num);
    return inb(CMOS_DAT);
}

void set_rtc_register(u16 reg, u8 val) {
    outb(CMOS_CMD, reg);
    outb(CMOS_DAT, val);
}

void rtc_read() {
    while (rtc_updating());

    crt_time.second = get_rtc_register(RTC_REG_SECOND);
    crt_time.minute = get_rtc_register(RTC_REG_MINUTE);
    crt_time.hour   = get_rtc_register(RTC_REG_HOUR);
    crt_time.day    = get_rtc_register(RTC_REG_DAY);
    crt_time.month  = get_rtc_register(RTC_REG_MONTH);
    crt_time.year   = get_rtc_register(RTC_REG_YEAR);

    u8 regb = get_rtc_register(0x0B);
    if (!(regb & 0x04)) {
        crt_time.second = (crt_time.second & 0x0F) + ((crt_time.second / 16) * 10);
        crt_time.minute = (crt_time.minute & 0x0F) + ((crt_time.minute / 16) * 10);
        crt_time.hour   = (crt_time.hour   & 0x0F) + (((crt_time.hour & 0x70) / 16) * 10) | (crt_time.hour & 0x80);
        crt_time.day    = (crt_time.day    & 0x0F) + ((crt_time.day / 16) * 10);
        crt_time.month  = (crt_time.month  & 0x0F) + ((crt_time.month / 16) * 10);
        crt_time.year   = (crt_time.year   & 0x0F) + ((crt_time.year / 16) * 10);
    }
}

void rtc_write(rtc_time_t *rt) {
    while (rtc_updating());

    set_rtc_register(RTC_REG_SECOND, rt->second);
    set_rtc_register(RTC_REG_MINUTE, rt->minute);
    set_rtc_register(RTC_REG_HOUR, rt->hour);
    set_rtc_register(RTC_REG_DAY, rt->day);
    set_rtc_register(RTC_REG_MONTH, rt->month);
    set_rtc_register(RTC_REG_YEAR, rt->year);
}

void init_rtc() {
    asm volatile("cli");
    outb(CMOS_CMD, 0x8A);
    outb(CMOS_DAT, 0x20);
    outb(CMOS_CMD, 0x8B);

    u8 prev = inb(CMOS_DAT);
    outb(CMOS_CMD, 0x8B);
    outb(CMOS_DAT, prev | 0x40);
    
    rtc_read();
    crt_time.century = 21;
}

static int is_leap_year(int year, int month) {
    if (year % 4 == 0 && (month == 1 || month == 2)) {
        return 1;
    }
    return 0;
}

static int get_weekday_from_date(rtc_time_t *rt) {
    char month_arr[] = {0x0,0x3, 0x3, 0x6, 0x1, 0x4, 0x6, 0x2, 0x5, 0x0, 0x3, 0x5};
    char cent_arr[] = {0x4, 0x2, 0x0, 0x6, 0x4, 0x2, 0x0}; 

    rt->century = 21; //i dont think i will ever need to change that
    int year = (rt->year + (rt->year / 4)) % 7;
    int month = month_arr[rt->month - 1];
    int century = cent_arr[rt->century - 18];
    int leap_year = is_leap_year(rt->year, rt->month);

    int ret = (year + month + century + rt->day - leap_year) % 7;
    return ret;
}

void print_time(rtc_time_t *rt) {
    char *weekdays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    char *wday = weekdays[get_weekday_from_date(rt)];
    int year = (rt->century - 1) * 100 + rt->year;

    char *hour = (char *) kmalloc(3 * sizeof(char));
    char *min = (char *) kmalloc(3 * sizeof(char));
    char *sec = (char *) kmalloc(3 * sizeof(char));

    add_leading_zero(hour, rt->hour);
    add_leading_zero(min, rt->minute);
    add_leading_zero(sec, rt->second);

    printf("rtc: %d, %d %s %s:%s:%s", year, rt->day, months[rt->month - 1], hour, min, sec);
    kfree(hour);
    kfree(min);
    kfree(sec);
}

void print_crt_time() {
    rtc_read();
    print_time(&crt_time);
}