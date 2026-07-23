# RF Online WorldServer Protection System — Roadmap

## Phase 1: Core Protection (COMPLETED)

All fixes from 6.23_Guard 2 (SnooperGS) recreated directly in WorldServer C++ source. No external DLL dependency.

### 1.1 Anti-Hack Module
- Speed hack detection (movement delta validation per tick)
- Fly hack detection (Y-axis anomaly, gravity check)
- Wall hack detection (BSP collision validation)
- Invisible hack detection (stealth state verification)
- Position rubber-banding on violation

### 1.2 Anti-Dupe Module
- Transaction locking system (mutex-based, per-player)
- Covered transaction types: TRADE, TRUNK, POST, UNMANNED_TRADER, COMBINE, STORE
- Double-lock rejection with logging
- Auto-unlock on player disconnect (NetClose cleanup)
- UMT buy quantity validation

### 1.3 Game Logic Fixes
- Honor Guild system fix (original vs custom mode)
- Vote system fix (per-account vote limit, content sanitization)
- Tower/Trap build restriction (level, license, map whitelist)
- Tower return after DC (items restored)
- Tower remote collect distance validation
- NPC Pager fix
- Character creation — one race per account option
- Character delete level range enforcement
- Teleport block during battle mode
- Free TP to HQ blocked for non-GM
- Race-locked item enforcement
- CW buff removal on guild leave
- Invalid buff cleanup after respawn
- Dual potion same-effect stacking prevention
- MAU key consumption fix
- Combine UI >5 slot crash prevention (server already caps at 5)
- CPT from same-race animus fix
- Post/Mail system toggle
- Max level, max dalant, max gold caps
- Chat level restriction (min level for all-chat)
- Character name type enforcement (alphanumeric vs special chars)
- Skip tutorial option

### 1.4 Input Sanitizer
- SQL injection prevention on all string inputs (char name, vote content, chat, mail)
- Buffer overflow protection (length validation before copy)
- Character name validation (regex-based, configurable type)
- Null-terminator enforcement on packet strings

### 1.5 Item Integrity
- Premium item validation (server-side verification against item table)
- Set item effect cap (configurable max_effects, default 6)
- Ore grade validation (cross-check mined ore vs allowed grade for player level/map)
- Embellish/upgrade validation

### 1.6 Combat Integrity
- Unit delivery WH/FH detection (MAU/Catapult height anomaly)
- Tower/Trap creation level + position validation
- Teleport cooldown enforcement
- Combat range validation (attack distance vs weapon range)

### 1.7 Anti-Cheat System
- Cheat scoring with progressive enforcement (warn → kick → ban)
- Score decay over time (rehabilitation)
- Packet rate limiting (configurable max packets/sec)
- HWID tracking and blocking (hardware ban)
- Periodic integrity check sweep (configurable interval)
- Per-player lifecycle tracking (enter/leave world)

### 1.8 Infrastructure
- INI-based config system (hot-reloadable via GM command)
- 10 config files in PROTECT/Configs/
- GM `dev_reload_protect` command
- Thread-safe singletons (std::mutex throughout)
- 8 dedicated log files per module
- ProtectionSystem manager wiring all 7 modules

### 1.9 Integration Points (12 source files)
- CMainThread.cpp — Initialize + OnTick
- CPlayer.cpp — CreateComplete (enter world), NetClose (leave), pc_MoveNext (movement)
- CNetworkEX.cpp — Packet flood detection, char name validation
- CPlayer_Trade.cpp — Trade lock/unlock
- CPlayer_Trunk.cpp — Trunk lock
- CPlayer_Post.cpp — Post lock
- CUnmannedTraderController.cpp — UMT buy validation + lock
- CPlayer_Mining.cpp — Ore grade validation
- CPlayer_Guild.cpp — Guild leave buff cleanup
- CPlayer_Combat.cpp — Tower creation validation
- CPlayer_Targeting.cpp — Teleport validation, vote sanitization
- CPlayer_Cheat.cpp — GM reload command

### 1.10 SQL Migration Scripts
- 001: Protection tables (HWID, cheat log, transaction log, config overrides)
- 002: BIGINT migration for Gold/Dalant, CHECK constraints, negative value cleanup
- 003: Stored procedures (HWID register/block, cheat logging, reports, log cleanup)

---

## Phase 2: Basic Free Modules (PLANNED)

Server-side gameplay features and client-side interface enhancements. Interface modules render via R3Engine hooks (same approach as Guard23). All configurable via INI, can be individually enabled/disabled.

### 2.1 Attack System
- Configurable damage increase/decrease multiplier per class, level range, or map
- PvP and PvE separate multipliers
- Skill damage modifier override table

### 2.2 Interracial Auction
- Cross-race auction house access (all 3 races share one auction)
- Configurable: full cross-race or selective (e.g. Bellato+Cora only)
- Race filter toggle in auction UI

### 2.3 Item Socket Gems
- Socket system for weapons/armor (configurable slot count per grade)
- Gem items with stat bonuses (attack, defense, HP, FP, SP)
- Socket/unsocket with success rate config
- Gem stacking rules and conflict resolution

### 2.4 7-Day Login Rewards
- Daily login gift table (configurable item/gold/dalant per day)
- 7-day cycle with reset
- Consecutive login bonus multiplier
- Missed day handling (reset or continue)

### 2.5 Static Boss Respawn Schedule
- Fixed respawn time per boss (instead of random window)
- Configurable per-monster respawn table
- Server-wide boss timer synchronization

### 2.6 Instant Item Charge
- Cash shop items activate immediately on purchase (no delay)
- Configurable per item type

### 2.7 HolyWar Chip Breaker Reward
- Configurable reward items/gold/dalant for chip destruction
- Per-race reward scaling
- Participation threshold (minimum damage/contribution to qualify)

### 2.8 TAB Target Toggle
- Config to enable/disable TAB targeting system
- Configurable target priority (nearest, lowest HP, same race enemy)

### 2.9 Skip Tutorial
- New characters skip tutorial map and spawn at race HQ
- Already implemented in GameFixes (config: `skipTutorial`)

### 2.10 Store with Ore/Process and Hunting Point
- Unmanned Trader accepts ore, processed materials, and hunting points as currency
- Configurable accepted currency types per store
- Exchange rate table (ore → gold equivalent)

### 2.11 Discord Integration
- Discord Rich Presence: show current character, race, level, map as activity status
- Server population display in Discord status
- Configurable display fields

### 2.12 Potion Replaced
- Replace potion drop/effect with currency (gold, dalant, hunting points)
- Configurable replacement table per potion type
- Auto-convert on pickup option

### 2.13 Potion Save
- Potion buff duration persists across logout/disconnect
- Remaining duration saved to DB on NetClose
- Restored on CreateComplete (player enter world)
- Configurable: enable/disable, max saved potions

### 2.14 Looting Fixed
- Fixed total loot amount per monster (configurable per monster type)
- Random vs fixed loot mode toggle
- Loot table weight adjustment

### 2.15 Looting Replaced
- Replace item drops with currency (gold, dalant, hunting points)
- Configurable replacement ratio per item grade
- Auto-pickup currency mode

### 2.16 Free For All (FFA) Looting
- Anyone can loot any monster drop (no kill ownership restriction)
- Configurable: FFA mode per map or global
- Loot timer before FFA kicks in (e.g. 30 sec for killer, then open)

### 2.17 Enchant System Enhanced
- Save on fail option (item not destroyed, stays at current level)
- Downgrade on fail (drop 1 level instead of destroy)
- Destroy on fail (original behavior)
- Configurable per upgrade level threshold
- Fail notification message to player
- Success/fail rate override table

### 2.18 Hardware Tracking / Blocked
- Already implemented in AntiCheat module (HWID tracking + blocking)
- Extended: multi-account detection via shared HWID
- HWID-to-account mapping report

### 2.19 Buffer Shop List
- NPC shop that sells buff scrolls/services
- Configurable buff list, duration, and price
- Race-specific buff shops

### 2.20 Party PvP Shared
- PvP kill rewards shared among party members
- Configurable share ratio (equal split, damage-based, proximity-based)
- Party contribution tracking

### 2.21 Disable Delete Character
- Configurable character deletion restriction
- Options: fully disabled, level range restriction, cooldown timer
- Already partially in GameFixes (`charDeleteMinLevel`, `charDeleteMaxLevel`)

### 2.22 Smart Box Open
- Auto-stack items when opening reward boxes
- If inventory has same item, stack automatically
- Overflow handling (mail or drop)

### 2.23 Monster Notification
- Server-wide announcement on boss monster death
- Rebirth/respawn notification (countdown or exact time)
- Configurable per monster ID

### 2.24 MAU WASD Mode
- Disable auto keyboard mode switch when entering MAU
- Keep WASD movement controls active inside MAU
- Configurable toggle

### 2.25 Reinstall Tower after Disconnect
- Tower items returned to inventory on DC (already in GameFixes: `towerReturnAfterDC`)
- Extended: auto-reinstall at same position on reconnect
- Configurable: return only, or return + reinstall

### 2.26 Candidate Registration
- Race leader candidate system with configurable conditions
- Payment requirement to register (gold/dalant/item)
- Level, guild rank, and playtime prerequisites
- Registration period window config

### 2.27 Zero Tax System
- Configurable option to set all tax rates to 0
- Per-race tax override
- Tax-free event mode (time-limited)

### 2.28 Vote System Enhanced
- Already implemented base vote fix in GameFixes
- Extended: vote conditions (level, guild membership, playtime)
- Vote reward for participation
- Configurable vote duration and result display

### 2.29 GM Command List Enhanced
- New GM commands with grade-based access control
- Configurable command permission table (command → required GM grade)
- Command logging and audit trail
- Custom command registration via config

### 2.30 Auto Regen CashPoint
- Automatic cash point regeneration per minute
- Configurable rate per player level or account tier
- Cap limit per day

### 2.31 Auto Active Magazine
- Automatically activate magazine charger if purse charger is in equip slot
- No manual activation needed
- Configurable: enable/disable

### 2.32 Auto Cure on Macro
- Auto-cure debuffs when macro system is active
- Configurable: which debuffs to auto-cure
- Cooldown between auto-cures

---

## Phase 3: Client Interface Modules (PLANNED)

All interface modules render via **R3Engine** hooks (same technique as Guard23). These modify the client rendering pipeline to display additional HUD elements, tooltips, and visual effects without modifying core game assets.

### 3.1 Display Chip Status
- Real-time chip war status overlay (HP percentage, race control)
- Minimap chip indicator

### 3.2 Display HEAL Text (Force/Class Skill)
- Floating text showing heal amount for force and class healing skills
- Color-coded by heal type (HP green, FP blue, SP purple)

### 3.3 Display Special Effect Description on Resource Item
- Tooltip overlay showing detailed effect description on resource/material items
- Parsed from item table data, rendered inline

### 3.4 Enable/Disable Right-Click Remove Buffs
- Config toggle to allow or prevent right-click buff removal
- Prevents accidental buff removal during combat

### 3.5 Display Field of View (FOV)
- Configurable camera FOV slider
- R3Engine camera parameter override
- Min/max FOV range to prevent exploit-level zoom

### 3.6 ScreenShake Toggle
- Option to enable/disable screen shake effects
- Configurable intensity multiplier
- Per-effect type toggle (explosion, hit, skill)

### 3.7 Interface: Delay Item/Skill
- Visual cooldown overlay on item and skill icons
- Circular sweep or numeric countdown display
- R3Engine texture overlay on action bar slots

### 3.8 Interface: GuardTower HP Percentage
- HP bar with percentage text displayed on placed guard towers
- Color gradient (green → yellow → red)
- Visible to tower owner and party members

### 3.9 Interface: Radar per Race
- Race-specific radar display showing nearby players/monsters
- Color-coded dots by type (friendly, enemy, NPC, monster)
- Configurable radar range

### 3.10 Interface: Enhanced Tooltips
- Extended tooltip information on hover (DPS calculation, set bonus preview, upgrade path)
- Comparison tooltip (equipped vs hovered item)
- R3Engine text rendering with custom fonts

### 3.11 Interface: Border Item by Grade
- Item icon border glow/color based on item grade
- Common (white) → Uncommon (green) → Rare (blue) → Epic (purple) → Legendary (gold)
- R3Engine shader overlay on inventory/equipment slots

### 3.12 Interface: Talic Item on Upgraded Item
- Display talic type icon on upgraded items in inventory
- Shows which talic was used for the upgrade
- Small icon overlay on item corner

### 3.13 Interface: Boss Respawn Schedule
- In-game window showing boss respawn timers
- Synced with server-side Static Boss Respawn (Phase 2.5)
- Countdown display with boss name, map, and spawn location

### 3.14 Interface: Kill and Death Notices
- Kill feed overlay (like FPS games)
- Shows killer name, victim name, weapon/skill used
- Configurable: PvP only, PvE included, all
- Fade timer and max displayed entries

### 3.15 Interface: News (Short Link)
- In-game news ticker / popup from server
- Server pushes news entries, client renders via R3Engine overlay
- Short link to external page (opens browser)
- Configurable display position, auto-hide timer

### 3.16 Interface: Status Indicator for Lock Bag
- Visual indicator showing locked bag slots
- Lock icon overlay on bag UI
- Color state (locked = red border, unlocked = normal)

### 3.17 Interface: Status Indicator for Purse Charger
- HUD indicator showing purse charger status (active, depleted, not equipped)
- Remaining charge display
- Auto-activate notification (ties into Phase 2.31)

### 3.18 New Quest Type: Kill Player / Patriarch
- Script-editable quest type targeting normal players or race patriarch
- Kill count tracking per quest instance
- PvP zone restriction option

### 3.19 New Quest Type: Guild Quests
- Script-editable guild-level quests
- Guild contribution tracking
- Reward distributed to guild bank or participating members

### 3.20 New Effect Generator
- Script-editable particle/visual effect system
- Custom effect triggers on item use, skill cast, or area entry
- R3Engine particle emitter configuration

### 3.21 New Limited Weapons for Specific Monsters
- Weapons that only deal damage to specific monster types
- Bonus damage multiplier against target monster
- Configurable weapon-monster mapping table

---

## Phase 4: Advanced Detection (PLANNED)

### 4.1 Advanced Speed Hack Detection
- Sliding window average speed (not just per-tick delta)
- Map-specific speed modifiers (mount zones, buff zones)
- Lag compensation with RTT measurement
- Statistical anomaly detection (z-score based)

### 4.2 Damage Hack Detection
- Server-side damage recalculation and comparison
- Damage cap per weapon type + level + buffs
- One-hit kill detection (damage vs target max HP ratio)
- Attack speed validation against weapon delay table

### 4.3 Auto-Potion / Macro Detection
- Input timing pattern analysis (perfect intervals = bot)
- Potion use frequency monitoring
- Skill rotation pattern detection
- AFK farming detection (no movement + continuous attack)

### 4.4 Memory Injection Detection
- Client-side CRC check packets (challenge-response)
- Known cheat tool signature detection via process list hash
- Client module integrity verification

### 4.5 Encryption & Packet Obfuscation
- XOR key rotation on packet payload
- Sequence number validation (replay attack prevention)
- Packet checksum (CRC32) on critical packets (trade, item, currency)

---

## Phase 5: Economy Protection (PLANNED)

### 5.1 Gold Sink / Source Monitoring
- Real-time gold supply tracking per race
- Alert system when total currency exceeds threshold
- NPC sell price anomaly detection

### 5.2 Trade Monitoring
- Suspicious trade pattern detection (one-sided high-value trades)
- Trade frequency limit per player pair
- Cross-race trade logging
- RMT detection heuristics (new account → immediate large gold transfer)

### 5.3 Auction House Protection
- Bid sniping prevention (minimum bid window)
- Price manipulation detection (buy own items at inflated price)
- Auction item duplication prevention
- Buyout price cap relative to item base value

### 5.4 Unmanned Trader Protection
- Price floor/ceiling validation
- Quantity limit per item type
- Store duration cap
- Tax evasion prevention

---

## Phase 6: Server Stability (PLANNED)

### 6.1 DDoS / Connection Flood Mitigation
- Per-IP connection rate limiting
- SYN flood detection at IOCP accept level
- Connection queue with priority (authenticated > new)
- Graceful degradation mode (reject new connections under load)

### 6.2 Crash Exploit Prevention
- Malformed packet fuzzing protection (bounds check on all packet fields)
- Stack buffer overflow guards on packet handlers
- Heap spray detection (repeated identical large allocations)
- Exception handler hardening (catch-all in packet dispatch)

### 6.3 Resource Exhaustion Prevention
- Per-player memory allocation cap
- Monster spawn rate limiting (prevent spawn bomb)
- Object count cap per sector
- Pathfinding query throttle

### 6.4 Database Protection
- Prepared statement migration (replace sprintf-based queries)
- Connection pool health monitoring
- Query timeout enforcement
- Dead connection recovery

---

## Phase 7: GM & Admin Tools (PLANNED)

### 7.1 In-Game GM Panel
- Real-time cheat score dashboard (top offenders)
- Player inspection (position, inventory hash, buff list)
- Teleport-to-player and summon
- Invisible observation mode
- Quick ban/unban with reason logging

### 7.2 Web Admin Dashboard
- HTTP API endpoint from WorldServer (or separate service reading DB)
- Live player count, gold supply, active alerts
- Cheat log viewer with filtering
- HWID management interface
- Config editor (push to tbl_protect_config, trigger reload)

### 7.3 Alert System
- Discord/Telegram webhook on ban-threshold events
- Daily cheat summary report (via stored procedure)
- Economy anomaly alerts
- Server health alerts (CPU, memory, connection count)

### 7.4 Audit Trail
- All GM actions logged with timestamp, target, reason
- Config change history
- Player punishment history with appeal status
- Immutable log (append-only table, no DELETE permission for game account)

---

## Phase 8: Client-Side Enhancements (PLANNED)

### 8.1 Client Integrity
- RF Online client file hash verification (dat/rfs file CRC)
- GameGuard/XTrap replacement (lightweight custom guard)
- Window title / process name randomization (anti-tool detection)
- Debug API detection (IsDebuggerPresent, NtQueryInformationProcess)

### 8.2 Anti-Screenshot / Recording
- Screen capture detection for competitive integrity
- Watermark player name in render (invisible to player, visible in screenshots)

### 8.3 Custom Launcher
- Auto-patcher with file integrity check
- Multi-client detection and prevention
- Forced fullscreen / window mode
- Server status display

---

## Priority Matrix

| Phase | Description | Impact | Effort | Priority |
|-------|-------------|--------|--------|----------|
| Phase 1 | Core Protection | Critical | Done | ✅ Complete |
| Phase 2 | Basic Free Modules (Server) | High | Medium | 🔴 Next |
| Phase 3 | Client Interface (R3Engine) | High | Medium | 🔴 Next |
| Phase 4 | Advanced Detection | High | Medium | 🟡 Important |
| Phase 5 | Economy Protection | High | Medium | 🟡 Important |
| Phase 6 | Server Stability | High | High | 🟡 Important |
| Phase 7 | GM & Admin Tools | Medium | Medium | 🟢 Future |
| Phase 8 | Client-Side Enhancements | Medium | High | 🟢 Future |

---

## Config-Driven Architecture

All features use INI config files in `PROTECT/Configs/`. Every check can be individually enabled/disabled and tuned without recompilation. GM command `dev_reload_protect` applies changes at runtime.

Current config files: Player.ini, AttackSystem.ini, System.ini, Guild.ini, Tower.ini, UnmannedTrader.ini, Enchant.ini, Addon.ini, HardwareBlocked.ini, Client.ini
