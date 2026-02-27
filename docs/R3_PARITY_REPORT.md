# R3 Parity Progress Report

## Current Commit
- Branch: `main`
- Commit: `9f04d8c`
- Working tree note: local uncommitted parity edits exist in:
  - `MapEditor/Viewer/MapViewerControl.cs`
  - `docs/R3_PARITY_REPORT.md`

## Renderer Mode Tested
- `LegacyCompat` mode wired and default.
- `R3Parity` mode wired as experimental branch entry.
- Verification this session:
  - `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
  - `dotnet run --project .\MapEditor\MapEditor.csproj -- --bsp "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp"` starts and stays running (manual timeout hit, no immediate CLI parse error).

## Repro Command
```powershell
dotnet run --project .\MapEditor\MapEditor.csproj -- --bsp "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp"
```
Optional explicit EBP:
```powershell
dotnet run --project .\MapEditor\MapEditor.csproj -- --bsp "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp" --ebp "D:\Private Files\playrf\RFOnline\Map\neutralB\NeutralB.ebp"
```

## What Changed (Latest Baseline)
1. Viewer core is functional with map folder discovery + combobox map switching.
2. BSP/EBP and R3M/R3T/R3X loading is in place.
3. R3E entity loading + RPK cache warmup is in place.
4. SPT entity references and alpha overrides are parsed.
5. Sky source switching is present.
6. Collision wall and grid toggles are separated.
7. Movement speed control and spawn reset behavior are in place.
8. Feature toggles exist (`Sky`, `R3T`, `R3M`, `R3E`, `R3X`, `DynLight`, `FxMark`).
9. Added dual renderer enum `RenderPipelineMode` with two modes:
   - `LegacyCompat`
   - `R3Parity`
10. `MapViewerControl.Render()` now routes through explicit mode branches:
    - `RenderLegacyCompat()`
    - `RenderR3Parity()`
11. Added MainForm pipeline UI dropdown (`Pipe`) to switch renderer at runtime.
12. Added active pipeline text in load status line (`Pipe: ...`) and control help text.
13. Added parity foundation scene model types in `BspStructures`:
    - `EntitySceneData`
    - `EntitySceneModel`
    - `EntitySceneInstance`
    - `EntitySceneObject` and track structs
    - `EntitySceneMatGroup`
14. `LoadedMap` now carries `EntityScene`.
15. Loader now emits runtime scene data from `.r3e` alongside legacy flattened entity mesh:
    - object track data preserved
    - per-mat-group local triangles preserved
    - instance transform list preserved
    while legacy mesh output remains unchanged.
16. Build checkpoint after these changes: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
17. Added deterministic model iteration + `MaterialBase` mapping in `EntitySceneModel` so parity path can resolve scene mat-group material IDs against merged entity material arrays.
18. Added first runtime parity entity render path in `MapViewerControl` (`R3Parity` mode):
    - per-frame `aniFrame = time * 30` sampling
    - hierarchical object matrix evaluation
    - per-mat-group local vertex transform by object matrix + instance transform
    - dynamic parity entity VBO upload and draw with non-legacy layer path
    while preserving existing legacy entity rendering path.
19. Build checkpoint after parity runtime path: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
20. Updated layer-span construction to preserve env-bump secondary layer metadata (primary + secondary layer pairing) for parity shader path on map/entity/parity-entity draw spans.
21. Build checkpoint after env-bump span update: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
22. Added parity runtime counters in `MapViewerControl`:
    - `ParityEntityDrawCalls`
    - `ParityAnimatedObjectUpdates`
    - `ParitySkippedLayers`
    and populated them in parity entity frame construction.
23. Build checkpoint after parity counter instrumentation: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
24. Added runtime status refresh in `MainForm` to display parity counters in status bar while `R3Parity` mode is active.
25. Build checkpoint after status-bar parity telemetry update: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
26. Added first parity alpha ordering split in non-legacy mesh draw path:
    - pass 0: opaque spans (`BlendNone`)
    - pass 1: alpha spans (all non-opaque blend modes)
    to move toward old alpha-stack behavior while keeping legacy mode unchanged.
27. Build checkpoint after parity opaque/alpha split: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
28. Added parity transparent span distance sorting (back-to-front) in non-legacy draw path using mesh/world vertex positions:
    - map/entity static spans use loaded mesh vertices
    - parity dynamic entity spans use per-frame generated vertices
    to move from fixed span order toward alpha-stack parity.
29. Build checkpoint after parity alpha distance sorting: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
30. Fixed compile break from interrupted refactor by restoring missing `MeshPassFilter` enum in `MapViewerControl`.
31. Completed parity-entity runtime call-site transition:
    - replaced stale `DrawParityEntityMesh(...)` call with `PrepareParityEntityFrameMesh()` preparation path.
32. `RenderCore` now explicitly branches into staged non-legacy rendering:
    - parity opaque stage (map + entity/parity-entity)
    - parity alpha stage (map + entity/parity-entity)
    while keeping legacy draw path unchanged.
33. Build checkpoint after render-core parity staging fix: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
34. Implemented parity unified transparent submission stage:
    - added `TransparentDrawCommand` capture model
    - added `CollectTransparentDrawCommands(...)`
    - merged map + entity/parity-entity alpha spans into one command list sorted back-to-front by camera distance.
35. Added explicit span-order injection support in `DrawMesh(...)` (`forcedSpanOrder`) so the unified transparent stage can submit exact per-span order while reusing existing draw-state logic.
36. Build checkpoint after unified transparent stage: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
37. Switched default renderer mode to `R3Parity` in `MainForm` to avoid accidental startup in legacy-static behavior.
38. Added `NSFlip` toolbar toggle (`MapViewerControl.FlipNorthSouth`) with default ON to invert world Z on display and align north/south orientation with game expectations.
39. Applied display-axis conversion to map/entity/sky/parity-entity vertices and overlays (grid/collision/particle markers) so the axis flip affects all rendered layers consistently.
40. Added `_BLEND_OPPA` alpha-test parity in mesh shader path:
    - new uniforms `uAlphaTestEnabled` + `uAlphaRef`
    - discard for `outAlpha < 8/255` when alpha-test is active.
41. Enabled layer animation evaluation in legacy path too (texture/UV/alpha animation) to prevent “stale/static” waterfall/ring behavior when users temporarily switch from parity.
42. Build checkpoint after orientation + alpha-test + animation fallback fixes: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
43. Added parity alpha-stage performance guard:
   - keep global alpha-span merge/sort only for small scenes (`<= 256` alpha spans),
   - otherwise render alpha in two batched calls (map alpha + entity alpha) with per-mesh distance sort.
44. Reduced parity entity animation CPU cost by caching per-model object matrices once per frame (instead of recomputing matrices per instance).
45. Reduced parity dynamic VBO upload overhead:
   - added frame-tick throttle (`30 fps` animation tick reuse),
   - switched from per-frame full `BufferData` reallocation to reusable CPU upload buffer + `BufferSubData`.
46. Build checkpoint after parity FPS optimizations: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
47. Split parity entity rendering into static and animated paths:
   - static parity entity mesh is baked once on map load (`_parityStaticEntity*` buffers),
   - animated parity entity mesh is rebuilt per animation frame only for models with real tracks.
48. Added model animation classification (`_entitySceneModelHasAnimation`) + map-level animated-instance detection (`_hasAnimatedEntityInstances`) so non-animated entities no longer pay per-frame transform cost.
49. Updated parity render stages to draw static and animated entity geometry separately (opaque + alpha paths) while preserving existing material pass behavior.
50. Build checkpoint after static/animated parity split: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).
51. Investigated missing portal-center spinner (`neutralB`) and confirmed assets/tracks are present (`potal_light02`, `b_gate`) but entity layer ARGB alpha handling was fading effect geometry too aggressively compared to old R3 entity path behavior.
52. Added entity-specific legacy layer-color rule in `MapViewerControl`:
   - map path still uses `ResolveLegacyLayerColor(...)`,
   - entity paths now use `ResolveLegacyEntityLayerColor(...)` (preserve RGB tint, default alpha to `1.0` unless animated alpha flicker flag is used),
   - applied to both legacy entity mesh span build (`UploadMeshLayer(..., useEntityLayerColorRule: true, ...)`) and parity runtime entity span build (`AddParityMaterialSpans(...)`).
53. Build checkpoint after entity layer-color parity adjustment: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors). Launched viewer with `--bsp "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp"` for manual visual verification.
54. Portal-center follow-up: switched entity alpha-blended layers to unlit in parity/legacy-entity span generation (`UseVertexLighting=false` for non-opaque entity blend modes) so portal/effect layers are no longer attenuated by current dynamic-light approximation.
55. Build checkpoint after entity effect-unlit tweak: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors). Launched viewer with `--bsp "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp"` for manual verification.
56. Added in-view camera tooling:
   - top-left camera coordinate overlay (`Cam XYZ` + `Src XYZ`),
   - per-map camera position persistence via user settings,
   - toolbar XYZ + `Go` teleport controls for exact camera placement.
57. Added GL context resilience for viewer:
   - replaced direct `MakeCurrent()` usage with guarded `TryMakeCurrent()` paths so transient WGL context failures no longer crash with unhandled exceptions.
58. Added parity-debug HUD line for nearest entity (`id/name/dist/alpha/anim`) and nearest FX instance (`idx/id/name/dist/pos`) to speed visual triage.
59. NeutralB finding confirmed from runtime checks:
   - `potal01` (`id=41` / `id=48`) near city portal positions is static model content,
   - dungeon portal animation path works,
   - missing city portal spinner is likely effect/material runtime behavior, not missing object-track animation on `potal01`.
60. Implemented first SPT runtime path for particle map entities:
   - parser now reads additional directives from `.spt`: `num`, `pos`, `live_time`, `time_speed`, `gravity`, `start_power`, `start_scale`, `start_alpha`, `start_color`, `start_yrot`, `start_zrot`, `time`, `scale`, `alpha`, `color`, `power`, `yrot`, `zrot`, `no_billboard`, `y_billboard`, `z_billboard`, `z_front`, `create_time_epsilon`, `start_time_range`.
   - loader now carries parsed particle runtime data into `EntitySceneModel.ParticleRuntime`.
   - parity entity renderer now evaluates runtime particle instances per frame (spawn phasing, keyframe interpolation, billboard basis, dynamic tint/alpha) instead of drawing particle `.r3e` once as a static mesh.
61. Build checkpoint after SPT runtime integration: `dotnet build .\MapEditor\MapEditor.csproj -c Debug` succeeded (0 errors).

## What Broke / Remaining Gaps
1. Portal internals/rings are not parity-correct (animation/pipeline mismatch).
2. Portal field compositing is still wrong in some views.
3. Full old-R3 pass ordering (`PreRenderOneLayer`/`AfterRenderOneLayer`, alpha stack staging across all passes) is not yet parity-implemented.
4. Sky/environment behavior improved but still not full fixed-function parity.
5. Axis conversion for rendered geometry is now handled in viewer-space (`NSFlip`); ext-dummy fog transforms still remain source-space data and rely on camera source-conversion path.
6. City `potal01` motion/effects remain incomplete because its visible circling behavior appears to come from runtime effect/material systems rather than model object tracks.
7. SPT runtime is now partially implemented, but parity with old `CParticle` is not complete yet (flicker, collision, full emit-state machine, and several niche script directives still missing).

## What Passed
1. Assets are loading (models/textures/entities) for NeutralB.
2. Water and portal visibility regressions were reduced by legacy-compatible path.
3. RPK caching is active and usable.

## Phase Status Checklist
- Phase 0 (baseline/report): In progress (report established).
- Phase 1 (dual renderer architecture): Core complete (toggle + branch routing in place).
- Phase 2 (entity runtime animation foundation): In progress (scene preservation + runtime draw path implemented; visual tuning/validation pending).
- Phase 3 (material pass builder): In progress (env-bump secondary layer metadata + initial parity opaque/alpha pass split).
- Phase 4 (map alpha ordering parity): In progress (opaque/alpha staged render + unified transparent stack implemented; visual fidelity tuning still pending).
- Phase 5 (sky/environment parity): Partial groundwork, not accepted.
- Phase 6 (NeutralB acceptance): Not started.

## Current Blocker
- Renderer currently mixes legacy-preserving behavior and partial shader emulation, so visibility is present but parity fidelity (portal/effect compositing and animation) is still incorrect even after unified transparent ordering.

## Next Immediate Step
1. Validate `potal_point02.spt` visuals at city portal after runtime parser/render update (ring visibility, motion cadence, tint).
2. Add missing old-`CParticle` behavior needed for parity (`flicker`, additional track flags, and emit-state timing edge cases).
3. Continue old-R3 pass sequencing parity (`PreRenderOneLayer`/`AfterRenderOneLayer` semantics, env-bump special path tuning).

## Next 3 Exact Tasks
1. Capture portal-front screenshots with HUD visible at `potal01` and dungeon portal to compare `Nearest Ent` and `Nearest FX` context.
2. Correlate `potal01` location with `.spt`-driven effect references and verify whether those instances are rendered/culled.
3. Port missing blend/depth/fog-state combinations from old `r3render.cpp` for portal/effect layers once FX presence is confirmed.

## Reference Screenshots
- NeutralB portal front (missing parity internals/rings in current baseline).
- NeutralB water channel (visibility restored in legacy-compatible path).
- NeutralB sky up-shot (brightness/fog still not fully parity-matched).

## Session Handoff
- If interrupted, resume from this file first.
- Update this file in every parity-focused commit.
- Do not merge parity work without updating `Current Commit`, `What Changed`, and `Next Immediate Step`.
