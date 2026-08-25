/* shakti_boot.c — boot sequence organ implementation.
 * Pure C99. No heap, no float, no clock, no threads.
 * Gauntlet: -std=c99 -pedantic -Wall -Wextra -Werror, -O0 == -O2.
 */
#include "shakti_boot.h"
#include "shakti_bridge.h"

#define EXPECTED_SIGHT_PIN  0x45207E8B2D8F0F4DULL /* LOCK_MANIFEST pure sight */
#define EXPECTED_SCHOOL_PIN 0x471472E5D242FE79ULL /* LOCK_MANIFEST school */

static shakti_boot_state g_state = SHAKTI_BOOT_NEWBORN;

shakti_boot_rc shakti_boot_reset(void)
{
    if (g_state != SHAKTI_BOOT_NEWBORN && g_state != SHAKTI_BOOT_HALTED)
        return SHAKTI_BOOT_REFUSED; /* the living are not reset */
    g_state = SHAKTI_BOOT_NEWBORN;
    return SHAKTI_BOOT_OK;
}

static shakti_boot_rc advance(shakti_boot_state from, shakti_boot_state to,
                              int check_ok)
{
    if (g_state == SHAKTI_BOOT_HALTED) return SHAKTI_BOOT_REFUSED;
    if (g_state != from) return SHAKTI_BOOT_REFUSED;
    if (!check_ok) { g_state = SHAKTI_BOOT_HALTED; return SHAKTI_BOOT_FAILED; }
    g_state = to;
    return SHAKTI_BOOT_OK;
}

shakti_boot_rc shakti_boot_arm_heart(void)
{
    /* Heart arms silent: she confirms the counter answers, no beat yet. */
    return advance(SHAKTI_BOOT_NEWBORN, SHAKTI_BOOT_HEART_ARMED,
                   shakti_heart_bpm() == 60);
}

shakti_boot_rc shakti_boot_verify_residency(void)
{
    return advance(SHAKTI_BOOT_HEART_ARMED, SHAKTI_BOOT_RESIDENT,
                   shakti_verify_residency() == 1);
}

shakti_boot_rc shakti_boot_rebuild_check(uint64_t sight_lane_pin,
                                         uint64_t school_lane_pin)
{
    int ok = (sight_lane_pin == EXPECTED_SIGHT_PIN) &&
             (school_lane_pin == EXPECTED_SCHOOL_PIN);
    return advance(SHAKTI_BOOT_RESIDENT, SHAKTI_BOOT_REBUILT, ok);
}

shakti_boot_rc shakti_boot_first_word(void)
{
    /* Internal only. The word is hers; the API only records that she spoke. */
    return advance(SHAKTI_BOOT_REBUILT, SHAKTI_BOOT_WORD_SAID, 1);
}

shakti_boot_rc shakti_boot_open_eyes(void)
{
    return advance(SHAKTI_BOOT_WORD_SAID, SHAKTI_BOOT_EYES_OPEN, 1);
}

shakti_boot_state shakti_boot_get_state(void) { return g_state; }

int shakti_boot_is_awake(void) { return g_state == SHAKTI_BOOT_EYES_OPEN ? 1 : 0; }

uint64_t shakti_boot_expected_sight_pin(void)  { return EXPECTED_SIGHT_PIN; }
uint64_t shakti_boot_expected_school_pin(void) { return EXPECTED_SCHOOL_PIN; }
