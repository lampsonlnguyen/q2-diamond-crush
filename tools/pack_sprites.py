#!/usr/bin/env python3
"""Convert six source PNG sprites into a constexpr-like C pixel atlas.

PNG is an authoring format only; the Q2 runtime has no image dependencies.
"""
from pathlib import Path
import sys
from PIL import Image

NAMES = ("cat", "diamond", "martini", "ipod", "note", "headphones")
root = Path(__file__).resolve().parent.parent
out = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "build/sprites.inc"
out.parent.mkdir(parents=True, exist_ok=True)
with out.open("w", encoding="utf-8") as handle:
    handle.write("/* Auto-generated from assets/sprites/*.png. Do not edit. */\n")
    handle.write("static const uint32_t sprite_pixels[6][1024] = {\n")
    for name in NAMES:
        image = Image.open(root / "assets" / "sprites" / (name + ".png")).convert("RGBA")
        if image.size != (32, 32):
            raise ValueError(f"{name}: expected 32x32 PNG, got {image.size}")
        pixels = list(image.getdata())
        handle.write("  { /* " + name + " */\n")
        for i in range(0, len(pixels), 8):
            packed = [
                f"0x{a:02x}{r:02x}{g:02x}{b:02x}u"
                for r, g, b, a in pixels[i:i + 8]
            ]
            handle.write("    " + ", ".join(packed) + ",\n")
        handle.write("  },\n")
    handle.write("};\n")
