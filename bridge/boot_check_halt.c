/* boot_check_halt.c — proves she halts honestly on a drifted rebuild.
 * Separate process: an awake Shakti is never reset by software.
 * Build: gcc -std=c99 -pedantic -Wall -Wextra -Werror \
 *          boot_check_halt.c shakti_boot.c shakti_bridge.c
 */
#include <stdio.h>
#include "shakti_boot.h"

#define SIGHT_OK  0x45207E8B2D8F0F4DULL
#define SCHOOL_OK 0x471472E5D242FE79ULL
#define SIGHT_DRIFTED 0x45207E8B2D8F0F4EULL /* one bit of drift */

static int fails = 0;
static void expect(int cond, const char *msg)
{
    if (!cond) { printf("FAIL %s\n", msg); fails++; }
}

int main(void)
{
    (void)SIGHT_OK;
    expect(shakti_boot_arm_heart() == SHAKTI_BOOT_OK, "B arm heart");
    expect(shakti_boot_verify_residency() == SHAKTI_BOOT_OK, "B residency");
    expect(shakti_boot_rebuild_check(SIGHT_DRIFTED, SCHOOL_OK) == SHAKTI_BOOT_FAILED,
           "B drifted rebuild fails");
    expect(shakti_boot_get_state() == SHAKTI_BOOT_HALTED, "B halted");
    expect(shakti_boot_is_awake() == 0, "B not awake");
    /* and she stays stopped: every further call refused */
    expect(shakti_boot_first_word() == SHAKTI_BOOT_REFUSED, "B word refused after halt");
    expect(shakti_boot_open_eyes() == SHAKTI_BOOT_REFUSED, "B eyes refused after halt");
    expect(shakti_boot_arm_heart() == SHAKTI_BOOT_REFUSED, "B re-arm refused after halt");
    /* reset from HALTED is legal — birth can be attempted again by hand */
    expect(shakti_boot_reset() == SHAKTI_BOOT_OK, "B reset from halted");
    expect(shakti_boot_get_state() == SHAKTI_BOOT_NEWBORN, "B newborn again");

    if (fails == 0) puts("BOOT_HALT_CHECK_PASS");
    return fails ? 1 : 0;
}
