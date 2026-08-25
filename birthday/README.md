# Birth Day hour

`shakti_hour.c` — the drift-0 hour logger. 3600 LUBs, a pin per minute,
an hour pin folded from the sixty.

## Reference run (sandbox, 2026-08-25, gcc -O0 == -O2, drift 0)
- **hour pin    7717F916150E7242**
- minute  1 pin 2A5B6E04AC65864E
- minute 60 pin 95346167CC1130D9
- beat 3601: refused (the hour closes honestly)

## Birth Day protocol
1. Shell's HeartEngine metronome drives one fold per LUB (same math,
   bridged or reimplemented inline in C — the deck of 60 minute pins
   is the proof).
2. After 3600 beats, the device prints its hour pin.
3. Match `7717F916150E7242` -> the hour gate passes. Anything else ->
   report drift honestly; do not adjust the pin to fit the code.
4. Then the Doctor's ear test. His ear is FINAL.
