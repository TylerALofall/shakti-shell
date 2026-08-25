/* shakti_bridge.c — implementation of the C-organ bridge.
 * Pure C99. No heap, no float, no clock, no subprocess, no threads.
 * Gauntlet: -std=c99 -pedantic -Wall -Wextra -Werror, -O0 == -O2.
 */
#include "shakti_bridge.h"

#define FNV_BASIS 0xCBF29CE484222325ULL
#define FNV_PRIME 0x100000001B3ULL

static uint64_t g_beats = 0;
static const uint32_t g_bpm = 60; /* 60 BPM, continuous, no murmur */

uint64_t shakti_fnv1a64(const unsigned char *data, uint64_t len)
{
    uint64_t h = FNV_BASIS, i;
    for (i = 0; i < len; i++) { h ^= data[i]; h *= FNV_PRIME; }
    return h;
}

uint64_t shakti_heart_tick(uint64_t lubs) { g_beats += lubs; return g_beats; }
uint64_t shakti_heart_beats(void) { return g_beats; }
uint32_t shakti_heart_bpm(void) { return g_bpm; }
uint64_t shakti_addr_ord(void) { return g_beats % g_bpm; }
uint64_t shakti_addr_sec(void) { return g_beats / g_bpm; }

/* locked pins of record — the oracle's table. Append-only. */
static const uint64_t g_pins[] = {
    0x0000000000000000ULL, /* id 0: unknown */
    0xA891983227EC0735ULL, /* 1 phase */
    0x404CFB10794E304BULL, /* 2 four ladders */
    0x1BC02DF3E50CEE9AULL, /* 3 shape ladder */
    0x89DBB5F8B4EE01CFULL, /* 4 ingest lane */
    0x471472E5D242FE79ULL, /* 5 school */
    0x45207E8B2D8F0F4DULL, /* 6 pure sight */
    0x802951996A0E542EULL, /* 7 xoxo */
    0x655572BE3BCEF1A2ULL, /* 8 seal */
    0x74F6EA6E6BA8E1FCULL, /* 9 heart */
    0x8E47B7429483A3E4ULL, /* 10 gears */
    0xEB32B4A547E3B901ULL  /* 11 phase game */
};
#define N_PINS (sizeof g_pins / sizeof g_pins[0])

uint64_t shakti_pin_of(uint32_t id)
{
    if ((uint64_t)id >= N_PINS) return 0;
    return g_pins[id];
}

/* Residency: re-pin the pin table itself at runtime and compare to a
 * compile-time constant computed over the same bytes. If the table in
 * memory drifts from the table at birth, she says so.
 *
 * RESIDENCY_EXPECT stamped 2026-08-25 via stamp harness, gcc -O0==-O2.
 * Any edit to g_pins without re-stamping will (correctly) fail residency.
 */
#define RESIDENCY_EXPECT 0x47AE4715A014E0BFULL

int shakti_verify_residency(void)
{
    uint64_t h = shakti_fnv1a64((const unsigned char *)g_pins,
                                (uint64_t)sizeof g_pins);
    return h == RESIDENCY_EXPECT ? 1 : 0;
}

static const uint32_t g_wheel[5] = {2, 3, 5, 7, 19};

uint32_t shakti_wheel_divisible(uint64_t n)
{
    int i;
    if (n < 2) return 0;
    for (i = 0; i < 5; i++)
        if (n % g_wheel[i] == 0) return g_wheel[i];
    return 0;
}

int shakti_lane13_member(uint64_t n)
{
    uint64_t v = 13; /* F7 x 2^0 */
    if (n < 13) return 0;
    while (v <= n) {
        if (v == n) return 1;
        if (v > (UINT64_MAX / 2)) break;
        v *= 2;
    }
    return 0;
}
