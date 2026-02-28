# RF MapEditor V1 Scope Lock

## Objective
Deliver a reliable RF-native map editor focused on practical production use:
1. Accurate map viewing.
2. Safe collision editing.
3. Predictable save/export for server-side boundary control.
4. NeutralB-first render parity progress without breaking editing workflow.
5. Keep map/BSP production authority inside this editor (not external DCC round-trip).

## Tooling Strategy (Locked)
1. Primary map pipeline is this MapEditor for `bsp/ebp/r3*` load/edit/save behavior.
2. `cbb-rf-online-addon` is reference-only for map behavior/inspection and quick visual cross-checks.
3. `cbb-rf-online-addon` remains valid for character/object asset workflows (`.bn/.msh/.ani`), which are separate from map-authoritative BSP editing.
4. Blender/OBJ round-trip is not an authoritative acceptance path for map validity in V1; client-safe RF serialization in this editor is the source of truth.

## Scope Lock (Do/Do Not)

### In Scope for V1
1. File loading:
   - `bsp`, `ebp`, `r3m`, `r3t`, `r3x`, `r3e`, `spt`.
2. File saving:
   - Authoritative collision save to `ebp`.
   - `Save As` for map pair flow where edited collision is persisted safely.
3. Collision editing:
   - Mouse wall draw.
   - Snap/glue.
   - Add boundary walls.
   - Add wall at camera.
   - Segment select/delete.
   - Undo/redo.
4. Viewer controls:
   - Camera movement/look.
   - Teleport by XYZ.
   - Per-map camera position persistence.
5. Visualization toggles:
   - `Sky`, `R3T`, `R3M`, `R3E`, `R3X`, `DynLight`, `FxMark`, collision, grid.
6. Renderer modes:
   - `LegacyCompat` as safe fallback.
   - `R3Parity` as parity track.

### Out of Scope for V1
1. Full RF format authoring pipeline from Blender back to all RF formats (`bsp/ebp/r3*`) in one pass.
2. Full `r3*` compiler/rebuilder toolchain.
3. Complete old-client particle/effect parity for every script edge case.
4. Full terrain/map geometry sculpting editor UX comparable to DCC tools.
5. Multiplayer/editor collaboration features.
6. Making external Blender tooling the primary BSP/map save path for production maps.

## V1 Acceptance Gate
V1 is accepted only when all conditions below are true:
1. Collision edits made in viewer are saved and reload identically from exported `ebp`.
2. Added/deleted segments preserve index integrity (no broken line references).
3. Snap/glue behavior is stable and deterministic for repeated edits.
4. Undo/redo works for all collision edit actions in scope.
5. NeutralB can be loaded from map root flow and switched from combobox without crash.
6. `LegacyCompat` and `R3Parity` modes can switch at runtime without reload failure.
7. Critical overlays and toggles update correctly without stale state.
8. Baseline manual scenarios pass (see checklist section).

## File Parity Checklist (V1)

### `ebp` (Primary)
1. Read: stable.
2. Edit: stable.
3. Write: stable.
4. Round-trip: required for V1 acceptance.

### `bsp`
1. Read: required.
2. Write: limited/supportive only for current pair-save flow.
3. Full geometry authoring parity: deferred beyond V1.
4. Validation target: files produced by this editor must be accepted by real RF client runtime, not only by in-editor renderer.

### `r3m`
1. Read: required.
2. Material behavior parity: partial in V1, improved incrementally.
3. Write: deferred.

### `r3t`
1. Read: required.
2. Texture usage parity: partial in V1.
3. Write: deferred.

### `r3x`
1. Read: required.
2. Environment parity: partial in V1.
3. Write: deferred.

### `r3e`
1. Read: required.
2. Runtime entity animation parity: in progress under `R3Parity`.
3. Write: deferred.

### `spt`
1. Read: required.
2. Runtime effect parity: partial and iterative in V1.
3. Write: deferred.

## Feature Checklist (V1)

### Must Keep Stable
1. Map folder scan and map combobox switch.
2. Wall draw with snap and bury controls.
3. Segment select/delete.
4. Undo/redo.
5. Save As export flow for edited map.
6. Camera teleport and persisted last position per map.
7. Overlay and renderer toggles.

### Nice-to-Have (Only if No Regression Risk)
1. Additional collision UX polish (selection clarity, affordances).
2. Additional diagnostics in HUD/status.
3. Performance optimization for parity path.

## Manual Regression Scenarios (Release Checklist)
1. Load NeutralB from map root folder picker.
2. Draw 3 connected walls with snap; verify no visible seam.
3. Delete middle segment; re-add segment; verify snap and continuity.
4. Undo and redo above operations.
5. Save map pair, reload, verify collision geometry unchanged.
6. Toggle collision/grid/sky/r3 toggles and verify immediate visual updates.
7. Switch renderer mode during runtime and continue editing.
8. Use teleport XYZ, close app, reopen map, verify camera restore.

## Phase Order
1. Stabilize collision edit/write pipeline first (`ebp` authority).
2. Stabilize editor UX/selection/undo behavior.
3. Harden BSP save validation against real client constraints and remove map-breaking attribute drift.
4. Continue `R3Parity` rendering phases without breaking edit pipeline.
5. Expand parity checks from NeutralB to two additional maps.

## Change Control Rule
Any new requested feature must be classified before implementation:
1. `In Scope V1`: can be implemented now.
2. `Out of Scope V1`: move to post-v1 backlog.
3. `Risky for Stability`: allowed only behind toggle or deferred.

## Single Source of Truth
1. Scope and acceptance: this file (`docs/MAP_EDITOR_V1_SCOPE.md`).
2. Parity execution details and session handoff: `docs/R3_PARITY_REPORT.md`.
