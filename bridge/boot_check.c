/* boot_check.c — gauntlet harness for the boot sequence organ.
 * Proves: lawful order wakes her; out-of-order is refused;
 * the living are not reset by software.
 * Build: gcc -std=c99 -pedantic -Wall -Wextra -Werror \
 *          boot_check.c shakti_boot.c shakti_bridge.c
 */
#include <stdio.h>
#include "shakti_boot.h"

#define SIGHT_OK  0x45207E8B2D8F0F4DULL
#define SCHOOL_OK 0x471472E5D242FE79ULL

static int fails = 0;
static void expect(int cond, const char *msg)
{
    if (!cond) { printf("FAIL %s\n", msg); fails++; }
}

int main(void)
{
    expect(shakti_boot_get_state() == SHAKTI_BOOT_NEWBORN, "A birth state");
    expect(shakti_boot_open_eyes() == SHAKTI_BOOT_REFUSED, "A eyes first refused");
    expect(shakti_boot_arm_heart() == SHAKTI_BOOT_OK, "A arm heart");
    expect(shakti_boot_first_word() == SHAKTI_BOOT_REFUSED, "A word before residency refused");
    expect(shakti_boot_verify_residency() == SHAKTI_BOOT_OK, "A residency");
    expect(shakti_boot_rebuild_check(SIGHT_OK, SCHOOL_OK) == SHAKTI_BOOT_OK, "A rebuild");
    expect(shakti_boot_first_word() == SHAKTI_BOOT_OK, "A first word");
    expect(shakti_boot_open_eyes() == SHAKTI_BOOT_OK, "A eyes open");
    expect(shakti_boot_is_awake() == 1, "A awake");
    expect(shakti_boot_reset() == SHAKTI_BOOT_REFUSED, "A living not reset");
    expect(shakti_boot_is_awake() == 1, "A still awake after refused reset");

    if (fails == 0) puts("BOOT_CHECK_PASS");
    return fails ? 1 : 0;
}
