# R3 Parity Implementation Plan

## Objective
Deliver NeutralB-first renderer parity against old R3 behavior while preserving a safe fallback renderer.

## Scope Lock
1. NeutralB is the gating map.
2. Keep dual renderer modes during migration:
   - `LegacyCompat` (default)
   - `R3Parity` (experimental)
3. Track all parity work in `docs/R3_PARITY_REPORT.md`.

## Baseline
- Baseline commit: `7818ab3` (`main`).
- Current behavior: legacy-compatible path keeps portal/water visible but parity gaps remain.

## Phases

### Phase 0: Baseline + Tracking
1. Preserve reproducible baseline behavior.
2. Maintain one progress report file with session handoff data.
3. No parity commit without report update.

Exit criteria:
1. Report file exists and is updated every parity session.
2. Repro command and screenshot references are listed.

### Phase 1: Dual Renderer Architecture
1. Add `RenderPipelineMode` enum (`LegacyCompat`, `R3Parity`).
2. Add runtime toggle in UI and status text showing active mode.
3. Keep legacy rendering path unchanged and default.

Exit criteria:
1. Runtime mode switching does not crash.
2. Legacy output remains baseline-equivalent.

### Phase 2: Entity Runtime Animation Parity Foundation
1. Stop relying only on frame-0 baked entity object transforms.
2. Preserve runtime-animatable entity model/object/track data from `.r3e`.
3. Evaluate entity object transforms per frame using old timing rule:
   - `aniFrame = (time + addFrame) * 30.0f` equivalent.
4. Render entity mat-groups with instance world * object frame matrix.

Exit criteria:
1. Portal internals/rings animate in NeutralB.
2. Entity sub-part motion is visible frame-to-frame.

### Phase 3: Fixed-Function Material Pass Builder
1. Recreate old pass ordering (`PreRenderOneLayer` style sequence).
2. Map alpha/blend/depth/fog semantics from old alpha types.
3. Recreate UV behavior flags (scroll/rotate/scale/lava/env/metal).
4. Implement env-bump special-case ordering.

Exit criteria:
1. Portal energy/water color/transparency visually close to game.
2. No map/entity disappearance regressions.

### Phase 4: Map and Alpha Ordering Parity
1. Split opaque and alpha passes.
2. Implement lightmap/detail layering order explicitly.
3. Build alpha stack and sort by camera distance.

Exit criteria:
1. Water stays visible from top and near-surface views.
2. Transparent surfaces remain stable while moving camera.

### Phase 5: Sky and Environment Parity
1. Match old sky draw state order (depth/fog/camera translation rules).
2. Integrate R3X environment hooks (fog and related behavior).
3. Preserve source switch behavior (`sky2`, fallback `sky`) in parity mode.

Exit criteria:
1. No pure black/white sky failures.
2. Sky brightness/fog transitions are close to in-game.

### Phase 6: NeutralB Acceptance + Broaden
1. Lock NeutralB validation camera presets:
   - Portal front
   - Water channel
   - Sky up-shot
2. Validate parity checklist and freeze acceptance.
3. Expand to 2 additional maps.

Exit criteria:
1. NeutralB checklist fully green.
2. Both renderer modes usable.

## Validation Matrix

### Automated
1. Loader coverage for `.bsp/.ebp/.r3m/.r3t/.r3x/.spt/.r3e`.
2. Pass-builder mapping tests.
3. Entity track sampling tests (frame updates change transforms).
4. Toggle regression tests.

### Manual (NeutralB)
1. Portal internals visible and animated.
2. Portal cyan field + blending close to game.
3. Water visible in all key views.
4. Sky/fog stable and plausible.
5. Feature toggles independently functional.

## Rollout
1. Default mode remains `LegacyCompat` until NeutralB parity passes.
2. `R3Parity` marked experimental until acceptance.
3. Add parity counters:
   - Opaque draw calls
   - Alpha draw calls
   - Animated objects updated
   - Skipped materials/layers

## Session Rule
Always resume from `docs/R3_PARITY_REPORT.md`, not memory.
