# FireRed / LeafGreen port (work in progress)

Goal: the Emerald 3DS experience — native ARM11 build, touch interface on the
bottom screen, optional voxel overworld — on Pokémon FireRed, built on
[pret/pokefirered](https://github.com/pret/pokefirered) instead of
pret/pokeemerald. Nothing here runs yet.

```sh
python tools/bootstrap.py --game firered   # pinned pokefirered + patches -> build/upstream-firered
```

`upstream.lock` `[pokefirered]` pins the decompilation. Bootstrap fetches it,
runs `tools/frlg/asm_cpp_syntax.py` over its assembly (asm/macros, constants,
data, sound) and applies `patches/pokefirered/*.patch` in order. The Emerald
build (`--game emerald`, the default) is unchanged.

## How the Emerald patches were carried over

| Emerald patch | FireRed | How |
|---|---|---|
| 0001 build system and tools | `0001-build-system-and-tools.patch` | Applied by hand. FireRed's `mapjson` already emits `#define` map ids and its preproc has no enum line-skip, so those two changes are not needed. |
| 0002 assembler and data syntax | `asm_cpp_syntax.py` + `0002-...patch` | The bulk of 0002 is mechanical (`@` comments to `/* */`, `.4byte`/`.word`/`.2byte` to `.int`/`.short`) because `full.mk` runs the data through cpp: it is a script now, checked against Emerald's 0002 (no residual differences besides upstream drift). The rest: `script_data` sections without `%progbits`, `gSpecials` made `.global`, the music players' track arrays moved to C. |
| 0003 portable audio, 0004 platform layer | `0003-portable-audio-and-platform-layer.patch` | Three-way merge (Emerald original as base, Emerald 3DS as theirs, FireRed as ours). m4a is nearly identical in both games. `global.h` keeps FireRed's save structures; the port's additions (stdio, `SAFE_MOD`, `ALIGNED(4)` on `Time`/`Mail`/`OldMan` for the GBA save layout, the emulated interrupt registers) are applied to it. The GBA-only alignment asserts in `gba/macro.h` are off under `PORTABLE`. |
| 0005 port hooks | `0005-port-hooks-wip.patch` | **In progress.** Three-way merge into the 147 files FireRed shares; 54 Emerald-only files (PokéNav, contests, Battle Frontier, ...) skipped. 527 conflicts; see below. |
| 0006+ features and translations | — | Not started. |

### 0005 status

`build/firered`-style compile of every game unit with the bridge flags
(`-DPORT_BRIDGE -DPLATFORM_3DS -DFIRERED -DREVISION=0 -DENGLISH`):
**270 of 291 units compile**. Remaining:

- Field engine, partly ported (Emerald hunks applied with `patch -F3`, the
  failed ones kept in `patches/pokefirered/rejects/`): `fieldmap.c`,
  `field_camera.c`, `bg.c`, `overworld.c`, `event_object_movement.c`,
  `field_effect.c`, `field_weather.c`, `scrcmd.c`. These carry the asset
  pointer resolution (`Port_ResolveAssetPointer`) that tileset and metatile
  reads need, so they must be ported, not dropped.
- Battle and misc: `battle_anim_effects_1.c`, `battle_anim_mons.c`,
  `battle_interface.c`, `battle_setup.c`, `pokeball.c`, `intro.c`.

Touch-UI hooks of Emerald's own screens were taken out for now
(`pokedex.c`, `item_menu.c`, `party_menu.c`, `start_menu.c`, `shop.c`,
`slot_machine.c`, `region_map.c`, `list_menu.c`, `evolution_scene.c`): FireRed
draws those screens differently (its Pokédex is `pokedex_screen.c`, its bag
`bag.c`/`item_menu.c` with a TM case and berry pouch), so their bottom-screen
versions are new work.

After everything compiles, every port change must be checked against a whole
change block of Emerald's 0005: the merge left fragments (a stray `#endif` and
`}` in `new_game.c`) that compile but change behaviour.

## Milestones

1. **Boot, flat, centred** (current). Every screen compiled as a GBA stage
   (`-DCTR_GBA_STAGE`, `compat/ctr_gba_centred.h`): the 240x160 game centred on
   the top screen, no widened field, no voxel world, minimal bottom screen.
   Needs: the rest of 0005, a `GAME=firered` mode in `3ds_port/full.mk` (file
   lists, no contest scripts, the Emerald-only bridge units replaced by stubs),
   and the data pipeline (`gen_map_data.py`, script and song bundles) checked
   against FireRed's layout.
2. **Data pack from the player's ROM.** Builder recipe for FireRed (USA Rev 0
   first; Rev 1 and LeafGreen are a `GAME_VERSION`/`REVISION` away in the
   decomp), `rom.py` accepting BPRE.
3. **Bottom screen.** Party, bag (with TM case and berry pouch), Pokédex,
   trainer card, save, options — FireRed's screens, not Emerald's PokéNav.
4. **Widened field** (400x240) — the `fieldmap.c`/`field_camera.c` hooks.
5. **Voxel Kanto.** The voxel generators run on any map's art, but every
   modelled building, relief fix and interior in `3ds_port/assets/voxel/` is
   Hoenn's.
6. Extras (follower, day/night, wireless) and other languages.

## Notes

- Emerald's `CopyTrainerId` hook copies nothing under `PORTABLE` unless `src`
  is NULL (it only has the NULL branch); the FireRed version copies.
- The 3DS toolchain used here: devkitARM from the pinned image in
  `upstream.lock` `[devkitarm]` (also on `mirror.gcr.io`).
