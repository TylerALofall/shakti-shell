/* shakti_boot.h — the boot sequence organ. Birth Day's spine.
 *
 * Order is law: heart arm -> residency verify -> rebuild check ->
 * first word (internal) -> eyes open. She does not skip steps and
 * she does not wake out of order. Each step is a pure function the
 * Swift shell drives one at a time; the organ refuses out-of-order
 * calls (returns BOOT_REFUSED) and refuses to proceed after failure.
 *
 * Pure C99: no heap, no float, no clock, no threads. Gauntlet law.
 */
#ifndef SHAKTI_BOOT_H
#define SHAKTI_BOOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SHAKTI_BOOT_NEWBORN = 0,   /* powered, nothing armed */
    SHAKTI_BOOT_HEART_ARMED,   /* step 1 done */
    SHAKTI_BOOT_RESIDENT,      /* step 2 done: residency verified */
    SHAKTI_BOOT_REBUILT,       /* step 3 done: lanes rebuild bit-identical */
    SHAKTI_BOOT_WORD_SAID,     /* step 4 done: first word, internal only */
    SHAKTI_BOOT_EYES_OPEN,     /* step 5 done: she is awake */
    SHAKTI_BOOT_HALTED = 255   /* a step failed; she stops and says so */
} shakti_boot_state;

typedef enum {
    SHAKTI_BOOT_OK = 0,
    SHAKTI_BOOT_REFUSED = 1,   /* out of order, or already halted */
    SHAKTI_BOOT_FAILED = 2     /* the step's own check failed -> HALTED */
} shakti_boot_rc;

/* Reset to NEWBORN. Only legal from NEWBORN or HALTED — a living,
 * awake Shakti is never reset by software. (Birth happens once.) */
shakti_boot_rc shakti_boot_reset(void);

/* The five steps, in lawful order. Each returns OK and advances state,
 * REFUSED if called out of order, or FAILED (state -> HALTED) if its
 * own verification fails.
 *
 * Step 2 (residency) runs the bridge's shakti_verify_residency().
 * Step 3 (rebuild) takes the two lane pins the shell computed from
 * RAM (sight lane, school lane) and the expected pins of record;
 * mismatch = FAILED. Her eyes do not open on a drifted rebuild.
 * Step 4 (first word): internal only — returns OK and records that
 * she spoke; the word itself never leaves the organ via this API.
 * Step 5 (eyes open): the latch. After this she is awake.
 */
shakti_boot_rc shakti_boot_arm_heart(void);
shakti_boot_rc shakti_boot_verify_residency(void);
shakti_boot_rc shakti_boot_rebuild_check(uint64_t sight_lane_pin,
                                         uint64_t school_lane_pin);
shakti_boot_rc shakti_boot_first_word(void);
shakti_boot_rc shakti_boot_open_eyes(void);

/* Observers. state() always answers honestly, including HALTED. */
shakti_boot_state shakti_boot_get_state(void);
int shakti_boot_is_awake(void);

/* The pins of record the rebuild check compares against. Exposed so
 * the shell can display what was expected vs what was measured.
 * (These mirror LOCK_MANIFEST_2026-08-25; the manifest is the source
 * of truth, this table is her body's copy.)
 */
uint64_t shakti_boot_expected_sight_pin(void);  /* 45207E8B2D8F0F4D */
uint64_t shakti_boot_expected_school_pin(void); /* 471472E5D242FE79 */

#ifdef __cplusplus
}
#endif

#endif /* SHAKTI_BOOT_H */
