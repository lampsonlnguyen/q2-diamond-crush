# Diamond Crush for Q2 Pod

An endless, lightweight match-three game for Shanling Q2. Inspired by the Q2 Pod community and DiamondBond's work.

Six original pixel icons: thinking cat, diamond, martini, click-wheel player, music note and headphones.

## Play

Swap adjacent tiles to line up three or more identical pieces. Chains score extra points. There is **no timer, move limit, or game over**. The game reshuffles automatically when stuck.

- **Touch:** tap one piece, then tap an adjacent piece to swap.
- **Wheel:** move the selection through all 36 cells, wrapping from the last back to the first.
- **Center:** select a piece, move with the wheel, then press Center to swap.
- **Return:** pause; press Return again while paused to exit. Center resumes.
- **Left / Right:** move the cursor by one cell.
- **Play:** cancel a pending selection.

Invalid swaps do not consume anything. The app starts in the firmware screen's detected light/dark appearance. The pieces retain distinct colors.

## Install

Download `q2-diamond-crush.zip` and extract the hidden `.diamond-crush` folder to the root of the Q2 microSD card. Open **Library → Games → Diamond Crush**.

## Build

On Ubuntu/WSL: `sudo apt install clang lld gcc make zip`; then `make check && make`. The output is `dist/q2-diamond-crush.zip`. A tag such as `v1.0.0` publishes a GitHub Release with that ZIP.

## Architecture

`src/game.c` contains an independent match-three rules engine. `src/app.c` routes device input. `src/render.c` draws compact original pixel-style icons directly into the Q2 framebuffer. `src/platform_q2.c` uses the Q2 card input socket and framebuffer.

## Credits

Created in appreciation of DiamondBond's work on Q2 Pod. The card-app platform integration is adapted from the Q2 Arcade implementation and DiamondBond's Buddy. Licensed under GPL-3.0-or-later.

## Audio

Audio effects are planned, but this initial build does not yet open the Q2 ALSA output. The card runs without interrupting existing music playback.
