# Atoms bundle (SAT1)

Every sound she owns, every pin intact, one file.

## Format
`SAT1` — magic, count, index (name, size, pin per atom), then raw bytes
in index order. The header is pinned; each atom is re-pinned on verify.

## Tool
`shakti_atoms.c` — pure C99, gauntlet-clean (-O0 == -O2, DRIFT_0).

    shakti_atoms pack   bundle.sat a.wav b.wav ...
    shakti_atoms verify bundle.sat     # re-pins every atom, reports drift
    shakti_atoms list   bundle.sat     # prints the index

## Founder 59 (verified 2026-08-25)
`FOUNDER59_PINS.txt` — header pin **D64DE779B62539CB**, 59 atoms,
drift 0. His voice says thirteen: `39_13.wav` pin `694C012CA308F3C6`.

The bundle binary itself is regenerable from the source WAVs —
house law: the pin is the lock, not the file. On Xcode day the WAVs
(or the .sat) go into the app bundle; `verify` runs at first boot
and its result feeds `shakti_boot_rebuild_check`.

## The 159 heartbeat atoms
Regenerable from `eden_heartbeat.c` (locked, pin 74F6EA6E6BA8E1FC).
When regenerated on the iMac, they pack with this same tool and
their header pin joins the manifest.
