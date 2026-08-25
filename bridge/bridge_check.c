/* bridge_check.c — gauntlet harness for shakti_bridge.c.
 * Verifies heart/address/wheel/lane behavior deterministically.
 * Build: gcc -std=c99 -pedantic -Wall -Wextra -Werror bridge_check.c shakti_bridge.c
 */
#include <stdio.h>
#include "shakti_bridge.h"

int main(void)
{
    uint64_t h;
    int fails = 0;

    /* 1. heart: 0 at birth, +1 per tick, ord/sec arithmetic */
    if (shakti_heart_beats() != 0) { puts("FAIL birth beats"); fails++; }
    shakti_heart_tick(120); /* two minutes at 60 BPM */
    if (shakti_heart_beats() != 120) { puts("FAIL tick count"); fails++; }
    if (shakti_addr_sec() != 2) { puts("FAIL addr sec"); fails++; }
    if (shakti_addr_ord() != 0) { puts("FAIL addr ord"); fails++; }
    shakti_heart_tick(7);
    if (shakti_addr_ord() != 7) { puts("FAIL addr ord 7"); fails++; }
    if (shakti_heart_bpm() != 60) { puts("FAIL bpm"); fails++; }

    /* 2. pins of record */
    if (shakti_pin_of(SHAKTI_PIN_HEART) != 0x74F6EA6E6BA8E1FCULL) { puts("FAIL pin heart"); fails++; }
    if (shakti_pin_of(SHAKTI_PIN_PHASE) != 0xA891983227EC0735ULL) { puts("FAIL pin phase"); fails++; }
    if (shakti_pin_of(999) != 0) { puts("FAIL pin unknown"); fails++; }

    /* 3. wheel law: 104 -> 2 (binary lane), 13 not in wheel -> 0 */
    if (shakti_wheel_divisible(104) != 2) { puts("FAIL wheel 104"); fails++; }
    if (shakti_wheel_divisible(13) != 0) { puts("FAIL wheel 13"); fails++; }
    if (shakti_wheel_divisible(19) != 19) { puts("FAIL wheel 19"); fails++; }
    if (shakti_wheel_divisible(1) != 0) { puts("FAIL wheel 1"); fails++; }

    /* 4. binary lane: 13=F7, 104=F7x2^3 members; 12, 14, 100 not */
    if (shakti_lane13_member(13) != 1) { puts("FAIL lane 13"); fails++; }
    if (shakti_lane13_member(104) != 1) { puts("FAIL lane 104"); fails++; }
    if (shakti_lane13_member(12) != 0) { puts("FAIL lane 12"); fails++; }
    if (shakti_lane13_member(100) != 0) { puts("FAIL lane 100"); fails++; }

    /* 5. fnv basis sanity + residency (stamped 2026-08-25) */
    h = shakti_fnv1a64((const unsigned char *)0, 0);
    if (h != 0xCBF29CE484222325ULL) { puts("FAIL fnv basis"); fails++; }
    printf("residency check: %s\n", shakti_verify_residency() ? "PASS" : "UNSTAMPED");

    if (fails == 0) puts("BRIDGE_CHECK_PASS");
    else printf("BRIDGE_CHECK_FAILS %d\n", fails);
    return fails ? 1 : 0;
}
