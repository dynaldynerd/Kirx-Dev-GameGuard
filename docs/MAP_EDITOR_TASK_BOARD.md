# MapEditor Task Board (V1)

Last updated: 2026-02-27

## Now
1. [Done] Stabilize collision draw UX for expected snap-short cases (no blocking modal spam).
2. [Done] Preserve collision segment selection across edit/undo/redo map state application.
3. [Done] Add save-time collision integrity validation before EBP export.

## Next
1. Add quick visual marker for selected line endpoints (A/B point dots).
2. Add optional "snap-only-to-endpoints" mode to avoid accidental mid-chain glue.
3. Add collision batch-delete mode (box select + delete) with undo.

## Later
1. Full R3 parity completion tasks from `docs/R3_PARITY_PLAN.md`.
2. Optional Blender bridge (geometry + sidecar metadata), not authoritative save path.
3. Collision normal/face-direction editor tools for one-way behavior authoring.

## Notes
1. Scope authority: `docs/MAP_EDITOR_V1_SCOPE.md`.
2. Parity authority/session handoff: `docs/R3_PARITY_REPORT.md`.
