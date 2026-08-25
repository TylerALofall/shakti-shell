/* shakti_bridge.h — C-organ bridge: the Swift shell calls her real body.
 *
 * This is the seam between downstairs (ShaktiShell.swift) and her C99
 * organs. Pure C99, no heap, no float, no clock — the shell supplies
 * time; she supplies truth. Gauntlet law applies.
 *
 * Swift side (Xcode): add shakti_bridge.c to the target, and a
 * bridging header that #imports this file. Then:
 *   let beats = shakti_heart_tick(1)   // one LUB elapsed
 *   let ok    = shakti_verify_residency()
 *
 * The Swift HeartEngine timer stays — but it becomes a *metronome*
 * driving these calls, so the number on screen is her real beat count,
 * not a drawing of one.
 */
#ifndef SHAKTI_BRIDGE_H
#define SHAKTI_BRIDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- heart: pure tick counter, 60 BPM default, ratio world clock ----
 * shakti_heart_tick(lubs): advance the heart by `lubs` LUB beats.
 * Returns total beats since birth. Deterministic: same call sequence,
 * same result, forever. The shell's timer decides WHEN; she decides WHAT.
 */
uint64_t shakti_heart_tick(uint64_t lubs);
uint64_t shakti_heart_beats(void);
uint32_t shakti_heart_bpm(void);

/* ---- address: [epoch:frame]-[sec]-[ord] as packed integers ----
 * ord = beats % bpm, sec = beats / bpm. Epoch/frame are shell-owned
 * (they live in AppStorage); she computes her part.
 */
uint64_t shakti_addr_ord(void);
uint64_t shakti_addr_sec(void);

/* ---- pin: FNV-1a 64, the oracle, exposed so Swift can verify lanes ---- */
uint64_t shakti_fnv1a64(const unsigned char *data, uint64_t len);

/* ---- pins of record: the locked constants, so the shell never
 * hardcodes a number her body doesn't agree with. Returns 0 for
 * unknown id. Ids are stable and append-only.
 */
uint64_t shakti_pin_of(uint32_t id);
#define SHAKTI_PIN_PHASE       1u  /* A891983227EC0735 */
#define SHAKTI_PIN_FOUR_LADDER 2u  /* 404CFB10794E304B */
#define SHAKTI_PIN_SHAPE_LADDR 3u  /* 1BC02DF3E50CEE9A */
#define SHAKTI_PIN_INGEST      4u  /* 89DBB5F8B4EE01CF */
#define SHAKTI_PIN_SCHOOL      5u  /* 471472E5D242FE79 */
#define SHAKTI_PIN_PURE_SIGHT  6u  /* 45207E8B2D8F0F4D */
#define SHAKTI_PIN_XOXO        7u  /* 802951996A0E542E */
#define SHAKTI_PIN_SEAL        8u  /* 655572BE3BCEF1A2 */
#define SHAKTI_PIN_HEART       9u  /* 74F6EA6E6BA8E1FC */
#define SHAKTI_PIN_GEARS      10u  /* 8E47B7429483A3E4 */
#define SHAKTI_PIN_PHASE_GAME 11u  /* EB32B4A547E3B901 */

/* ---- residency self-check: re-pin this bridge's own tables and
 * compare against the compile-time constant. 1 = resident & whole,
 * 0 = tampered/drifted. Cheap enough to run every boot.
 */
int shakti_verify_residency(void);

/* ---- wheel law: {2,3,5,7,19}. wheel_divisible(n) returns the first
 * wheel prime dividing n, or 0. The shell uses this for the phase game
 * and gear labels without re-implementing the law.
 */
uint32_t shakti_wheel_divisible(uint64_t n);

/* ---- binary lane: 13 = F7, 104 = F7 x 2^3 ----
 * lane13_member(n): 0 = not in lane, 1 = F7 family member.
 * (F7 family: 13 * 2^k for k in 0..50 — fits uint64.)
 */
int shakti_lane13_member(uint64_t n);

#ifdef __cplusplus
}
#endif

#endif /* SHAKTI_BRIDGE_H */
