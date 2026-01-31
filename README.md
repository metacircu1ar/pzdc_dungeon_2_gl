# PZDC Dungeon 2 OpenGL Demo (pzdc_dungeon_2_gl)

This demo is a C/OpenGL port of the Ruby terminal version. It renders YAML-defined screens and ASCII art in an OpenGL glyph grid using SDL2 + SDL_ttf, and runs a full game loop with menus, battles, events, loot, and persistence.

## Dependencies (Ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  pkg-config \
  libsdl2-dev \
  libsdl2-ttf-dev \
  libyaml-dev \
  libgl1-mesa-dev \
  mesa-common-dev \
  fonts-dejavu-core
```

## Build

```bash
git clone https://github.com/metacircu1ar/pzdc_dungeon_2_gl.git
cd pzdc_dungeon_2_gl
make
```

## Run

```bash
./pzdc_dungeon_2_gl
```

If you want to override the font path:

```bash
./pzdc_dungeon_2_gl --font /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf
```

## Controls

- Number keys: choose menu options
- Letters (a, b, c...): inspect items/recipes when prompted
- Y/N: accept or decline loot
- Enter: confirm text input / advance when prompted
- Backspace: edit text input fields
- Esc: quit

## Architecture

- `main.c`: single translation unit containing the renderer, data loaders, and the full game state machine.
- Rendering: SDL2 creates the window and OpenGL context; SDL_ttf rasterizes glyphs into a texture atlas; the screen is drawn as a fixed grid of textured quads.
- Views: YAML screens in `views/menues/` and ASCII art in `views/arts/` are parsed via libyaml and composed at runtime with placeholder substitution.
- Data: YAML in `data/` defines heroes, enemies, dungeons, skills, items, events, shop inventory, and occult recipes.
- State machine: a `GameState` enum drives all flows (start, load, camp, battle, event, loot, shop, options, credits, etc.), with input handled per-state.
- Persistence: YAML saves under `saves/` for hero-in-run, monolith points, statistics, warehouse, shop, and occult library.
- Resources: the demo is self-contained under `pzdc_dungeon_2_gl/` with path resolution for data, views, assets, and saves.

## Implemented features

- libyaml for YAML parsing of menus, arts, and data.
- Implemented interactive mode by default with full keyboard input handling.
- Implemented end-of-run transfer (monolith points + camp loot to shop/warehouse) and wired it to save/exit, death, and victory flows.
- Implemented camp events and event screens (loot, gambler, altar, boatman, bridge keeper, warriors grave quest, pig with saucepan, black mage battle, exit run), including text input handling.
- Implemented enemy battle art animations (attack/damaged/dead) and screen transition effects, with adjustable animation speeds.
- Added save/resume flow and expanded persistence for event-related quest state.

## Remaining Work

- Verify parity with the original Ruby code and implement any missing screens/flows (for example, the unused `choose_pzdc_boss_screen.yml` flow is not wired).
- Add hero-side battle animation/movement frames if you want full character animation parity (current animations are enemy art swaps + screen transitions).
- Add audio/music support if desired.
- Improve configurability (key rebinding, fullscreen/vsync toggles, resolution presets).
- Add automated tests for data loading and game state transitions.
