# Diamond Crush for Q2 Pod

An endless, lightweight match-three game for Shanling Q2. Inspired by the Q2 Pod community and DiamondBond's work.

Six original pixel icons: thinking cat, diamond, martini, click-wheel player, music note and headphones.

## Play

Swap adjacent tiles to line up three or more identical pieces. Chains score extra points. There is **no timer, move limit, or game over**. The game reshuffles automatically when stuck.

- **Touch:** tap one piece, then tap an adjacent piece to swap.
- **Wheel:** move the selection through all 40 cells, wrapping from the last back to the first.
- **Center:** select a piece, move with the wheel, then press Center to swap.
- **Return:** pause; press Return again while paused to exit. Center resumes.
- **Left / Right:** move the cursor by one cell.
- **Play:** cancel a pending selection.

Invalid swaps do not consume anything. The app starts in the firmware screen's detected light/dark appearance. The pieces retain distinct colors. Light theme uses a dark navy, high-contrast board background with darker tiles and clearer borders; both themes share the 5-row by 8-column layout.

## Install

Download `q2-diamond-crush.zip` and extract the hidden `.diamond-crush` folder to the root of the Q2 microSD card. Open **Library → Games → Diamond Crush**.

## Build

On Ubuntu/WSL, install `clang lld gcc make python3 python3-pil zip`. The portable game tests run with `make check`, without any device dependencies.

For `make`, install the Q2 audio sysroot at `~/.cache/q2/sysroot`. Follow [Q2 Groovebox](https://github.com/DiamondBond/q2-groovebox) and use the upstream [Rockbox sysroot helper](https://github.com/DiamondBond/q2-rockbox/blob/main/tools/shanlingq2/mksysroot.sh). **That helper requires the exact official `Q2 Firmware V1.32.zip`** (its checksum and internal archive path are pinned to V1.32), even if your device is running Q2 Pod based on V1.36. An already-prepared Groovebox sysroot can be reused.

```sh
sudo apt install clang lld gcc make python3 python3-pil zip unzip curl xz-utils binutils squashfs-tools
git clone --depth 1 https://github.com/DiamondBond/q2-rockbox.git
sh q2-rockbox/tools/shanlingq2/mksysroot.sh "/path/to/Q2 Firmware V1.32.zip" "$HOME/.cache/q2/sysroot"
make check
make
```

The sysroot must contain `usr/include/alsa/asoundlib.h` and `lib/libasound.so.2`. Override its location with `make SYSROOT=/path/to/sysroot`. It provides the device's glibc, ALSA, pthreads and dynamic loader.

Run `make check && make` to create `dist/q2-diamond-crush.zip`. The MIPS binary has **no Python or PNG dependency on the Q2**; it does link against the firmware's existing shared audio libraries.

GitHub Actions runs portable tests, builds a Q2 sysroot from the checked-in `fw/Q2 Firmware V1.36.zip` using DiamondBond's Rockbox sysroot helper, and packages the installable game ZIP. Pushing a `v*` tag automatically creates a GitHub Release with `q2-diamond-crush.zip`. The old `Q2_SYSROOT_URL` variable is no longer required. **Stock firmware may contain proprietary code; do not redistribute the firmware ZIP or extracted libraries without permission.**

## Architecture

`src/game.c` contains an independent match-three rules engine. `src/app.c` routes device input. `src/render.c` renders sprites generated at build-time from individual 32×32 transparent PNG files in `assets/sprites/`. The build packs RGBA pixels into a C header, with no on-device PNG decoding. `src/platform_q2.c` uses the Q2 card input socket and framebuffer.

## Credits

Created in appreciation of DiamondBond's work on Q2 Pod. The card-app platform integration is adapted from the Q2 Arcade implementation and DiamondBond's Buddy. The combined program uses GPL version 3 because its ALSA device integration follows Groovebox (GPL-3.0-only); individual original source files retain their SPDX license headers.

## Audio

The app now requests the firmware's sound output with `audio=1` and plays four original effects:

- `select.wav` — select a tile or begin a swap
- `invalid.wav` — a swap that does not make a match
- `match.wav` — a successful match
- `combo.wav` — cascading matches

`src/audio.c` reads the PCM WAVs (44.1/48 kHz, mono or stereo, with optional metadata chunks) once from `.diamond-crush/sounds/`, mixes them to mono and resamples to 44.1 kHz stereo through ALSA, and accepts sound events without blocking touch or wheel input. It follows Groovebox's USB/Bluetooth output selection, soft-volume curve, DAC mute handling, and busy-PCM retry policy. **Q2 Pod 1.7.2 or later** is needed for the firmware audio handoff. If ALSA cannot open the device, the game remains playable without sound. Sound playback has not yet been tested on physical Q2 hardware.

The firmware stops normal music playback when handing the audio output to a sound-enabled card game.

## Art asset workflow

Each playable piece is a separate transparent 32×32 RGBA PNG under `assets/sprites/`, ordered by piece ID: cat, diamond, martini, iPod-style player, music note, headphones. Run `python3 tools/pack_sprites.py` to regenerate `build/sprites.inc`; `make` runs it automatically. Never edit generated C sprites by hand. All sprites use one pixel coordinate system; rendering uses alpha blending for light and dark themes. Only the PNG sources and packer are tracked; generated `build/` files are ignored. The original artwork was created for this project.

The board has **8 columns × 5 rows**. Swipe a piece up, down, left, or right to swap with its neighbor. Swaps slide with smooth easing; moves without a match slide back. Wheel/Center swaps use the same effect. The iPod artwork is fitted to its 32×32 tile without transparent padding.
