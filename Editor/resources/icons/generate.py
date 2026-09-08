#!/usr/bin/env python3
"""Draws the editor's icon set.

The icons are procedural rather than hand-drawn art: every one is a few primitives on a
32x32 grid, which keeps them consistent and keeps a tweak (a heavier stroke, a different
corner radius) to an edit here instead of a round trip through an image editor.

This is an offline tool. The build embeds the PNGs it writes, it never runs this - so
needing Pillow costs nothing to anyone who is only compiling the editor.

    python Editor/resources/icons/generate.py

Everything is drawn white on transparent, because the editor tints icons with the
theme's text colour at draw time and tinting multiplies: a black glyph stays black.
"""

from pathlib import Path

from PIL import Image, ImageDraw

# Icons are drawn at 8x and downsampled, which is what gives them their antialiasing -
# Pillow's draw operations are hard-edged.
SIZE = 32
SS = 8
N = SIZE * SS

# Stroke weight and padding, in the supersampled space. 16 is a 2px stroke at 32px,
# which stays legible once ImGui scales an icon down to the font height.
W = 16
PAD = 3 * SS

WHITE = (255, 255, 255, 255)


def canvas():
    image = Image.new("RGBA", (N, N), (255, 255, 255, 0))
    return image, ImageDraw.Draw(image)


def draw_add(d):
    """A plus."""
    a, b = PAD, N - PAD
    mid = N // 2
    d.rounded_rectangle([a, mid - W // 2, b, mid + W // 2], radius=W // 2, fill=WHITE)
    d.rounded_rectangle([mid - W // 2, a, mid + W // 2, b], radius=W // 2, fill=WHITE)


def draw_delete(d):
    """A waste basket: the lid, the handle above it and the tapered body below."""
    d.rectangle([96, 32, 160, 56], outline=WHITE, width=W)
    d.line([32, 64, 224, 64], fill=WHITE, width=W + 4)
    d.line([(64, 88), (76, 224), (180, 224), (192, 88)], fill=WHITE, width=W, joint="curve")
    d.line([110, 112, 106, 200], fill=WHITE, width=W - 2)
    d.line([146, 112, 150, 200], fill=WHITE, width=W - 2)


def draw_save(d):
    """A floppy disk: the case, the shutter on top and the label below."""
    d.rounded_rectangle([PAD, PAD, N - PAD, N - PAD], radius=W, outline=WHITE, width=W)
    d.rectangle([96, PAD, 160, 100], fill=WHITE)
    d.rectangle([76, 148, 180, N - PAD], outline=WHITE, width=W)


def draw_up(d):
    """An arrow pointing up, for leaving a folder."""
    d.line([N // 2, 224, N // 2, 72], fill=WHITE, width=W + 4)
    d.polygon([(N // 2, 28), (56, 116), (200, 116)], fill=WHITE)


def draw_folder(d):
    """A folder, tab on the left."""
    d.polygon(
        [(32, 72), (100, 72), (128, 104), (224, 104), (224, 208), (32, 208)],
        fill=WHITE,
    )


def draw_file(d):
    """A page with its corner folded over."""
    d.line(
        [(148, 32), (60, 32), (60, 224), (196, 224), (196, 80), (148, 32), (148, 80), (196, 80)],
        fill=WHITE,
        width=W,
        joint="curve",
    )


def draw_model(d):
    """A cube in isometric: the silhouette, then the three edges meeting at the centre."""
    hull = [(128, 28), (216, 78), (216, 178), (128, 228), (40, 178), (40, 78)]
    d.line(hull + [hull[0]], fill=WHITE, width=W, joint="curve")
    d.line([(40, 78), (128, 128), (216, 78)], fill=WHITE, width=W, joint="curve")
    d.line([128, 128, 128, 228], fill=WHITE, width=W)


def draw_scene(d):
    """A framed view with a sun and hills - a scene file holds what the viewport shows."""
    d.rounded_rectangle([PAD, 48, N - PAD, 208], radius=W, outline=WHITE, width=W)
    d.ellipse([80, 84, 116, 120], fill=WHITE)
    d.polygon([(48, 190), (108, 118), (152, 168), (188, 130), (208, 190)], fill=WHITE)


def draw_project(d):
    """Stacked layers: a project is the thing every other file sits inside."""
    d.polygon([(128, 24), (212, 72), (128, 120), (44, 72)], fill=WHITE)
    d.line([(44, 128), (128, 176), (212, 128)], fill=WHITE, width=W, joint="curve")
    d.line([(44, 180), (128, 228), (212, 180)], fill=WHITE, width=W, joint="curve")


# The name of each entry is the file it is written to, and so the symbol the build
# generates for it: 'add' becomes kAdd in EmbeddedIcons.h.
ICONS = {
    "add": draw_add,
    "delete": draw_delete,
    "save": draw_save,
    "up": draw_up,
    "folder": draw_folder,
    "file": draw_file,
    "model": draw_model,
    "scene": draw_scene,
    "project": draw_project,
}


def main():
    out = Path(__file__).parent

    for name, draw in ICONS.items():
        image, d = canvas()
        draw(d)

        image = image.resize((SIZE, SIZE), Image.LANCZOS)
        image.save(out / f"{name}.png", optimize=True)
        print(f"{name}.png")


if __name__ == "__main__":
    main()
