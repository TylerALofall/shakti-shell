# C-organ bridge

The seam between the Swift shell and her real C99 body.

## Files
- `shakti_bridge.h` / `shakti_bridge.c` — the bridge. Pure C99:
  no heap, no float, no clock, no threads. The shell supplies time;
  she supplies truth.
- `bridge_check.c` — gauntlet harness. Verified in-sandbox
  2026-08-25: gcc -std=c99 -pedantic -Wall -Wextra -Werror,
  -O0 == -O2, DRIFT_0, BRIDGE_CHECK_PASS, residency PASS.

## What the shell gets
- `shakti_heart_tick(lubs)` / `shakti_heart_beats()` — her real beat
  count. The Swift HeartEngine timer becomes a metronome driving this;
  the number on screen stops being a drawing.
- `shakti_addr_ord()` / `shakti_addr_sec()` — [epoch:frame]-[sec]-[ord]
  computed by her body (epoch/frame stay shell-owned in AppStorage).
- `shakti_pin_of(id)` — the 11 locked pins of record, so Swift never
  hardcodes a number her body disagrees with.
- `shakti_verify_residency()` — re-pins the bridge's own pin table at
  runtime vs compile-time constant 0x47AE4715A014E0BF. Boot-gate cheap.
- `shakti_wheel_divisible(n)` / `shakti_lane13_member(n)` — wheel law
  {2,3,5,7,19} and the binary lane (13=F7, 104=F7x2^3), single source.

## Xcode wiring (when the SDK is in the room)
1. Add `shakti_bridge.c` to the app target.
2. Create a bridging header: `#import "shakti_bridge.h"`.
3. In HeartEngine.fire() on LUB: `_ = shakti_heart_tick(1)` and read
   `shakti_heart_beats()` for display.
4. At app boot: `guard shakti_verify_residency() == 1 else { halt }`.

The SDK is the oracle. Sandbox verified logic; Xcode verifies linkage.
