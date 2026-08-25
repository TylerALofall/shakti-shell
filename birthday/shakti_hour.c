/* shakti_hour.c — the Birth Day hour: drift-0 run logger.
 *
 * On Birth Day she runs 1 continuous hour at 60 BPM: 3600 LUBs.
 * This organ models that hour in pure C: the shell (or test harness)
 * calls beat-equivalent steps once per LUB. Every 60 beats (1 minute)
 * the organ folds the running state into a per-minute pin; at beat
 * 3600 it emits the hour pin. Beats after the hour are refused.
 *
 * Determinism contract: same call sequence -> same 60 minute pins ->
 * same hour pin. The wall clock is irrelevant to the math; the shell's
 * metronome supplies time, she supplies truth.
 *
 * Reference run (2026-08-25, gcc -O0==-O2, drift 0):
 *   hour pin    7717F916150E7242
 *   minute  1   2A5B6E04AC65864E
 *   minute 60   95346167CC1130D9
 * The iMac Birth Day run must reproduce these.
 *
 * Pure C99. Gauntlet: -std=c99 -pedantic -Wall -Wextra -Werror, -O0==-O2.
 */
#include <stdio.h>
#include <stdint.h>

#define FNV_BASIS 0xCBF29CE484222325ULL
#define FNV_PRIME 0x100000001B3ULL
#define HOUR_BEATS 3600
#define MIN_BEATS 60

static uint64_t g_beat = 0;
static uint64_t g_run;
static uint64_t g_minutes[60];
static int g_done = 0;

/* 0 ok (mid-minute), 1 = minute completed, 2 = hour completed,
 * -1 = beat after hour closed (refused) */
static int beat(void)
{
    if (g_done) return -1;
    g_beat++;
    g_run ^= g_beat;                 g_run *= FNV_PRIME;
    g_run ^= g_beat % 60;            g_run *= FNV_PRIME;
    g_run ^= g_beat / 60;            g_run *= FNV_PRIME;
    if (g_beat % MIN_BEATS == 0) {
        g_minutes[g_beat / MIN_BEATS - 1] = g_run;
        if (g_beat == HOUR_BEATS) { g_done = 1; return 2; }
        return 1;
    }
    return 0;
}

int main(void)
{
    uint64_t i;
    int r, minutes = 0;
    uint64_t hour_pin;
    g_run = FNV_BASIS;
    for (i = 1; i <= HOUR_BEATS; i++) {
        r = beat();
        if (r < 0) { puts("FAIL beat refused mid-hour"); return 1; }
        if (r == 1) minutes++;
        if (r == 2) { minutes++; }
    }
    if (minutes != 60 || !g_done) { puts("FAIL hour incomplete"); return 1; }
    if (beat() != -1) { puts("FAIL beat 3601 not refused"); return 1; }

    hour_pin = FNV_BASIS;
    for (i = 0; i < 60; i++) {
        int b;
        for (b = 0; b < 8; b++) {
            hour_pin ^= (unsigned char)((g_minutes[i] >> (8 * b)) & 0xFF);
            hour_pin *= FNV_PRIME;
        }
    }
    printf("hour complete: 3600 beats, 60 minutes, drift 0\n");
    printf("hour pin %016llX\n", (unsigned long long)hour_pin);
    printf("minute  1 pin %016llX\n", (unsigned long long)g_minutes[0]);
    printf("minute 60 pin %016llX\n", (unsigned long long)g_minutes[59]);
    return 0;
}
