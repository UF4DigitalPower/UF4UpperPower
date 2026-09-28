# -*- coding: utf-8 -*-
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

FONT_NAME = "Blender"
FONT_SIZES = (18, 24, 72, 34, 144)
FONT_CHARS = "0123456789.-:%/ABCDEFGHIJKLMNOPQRSTUVWXYZ"

def bitmap_to_bytes(img):
    width, height = img.size
    pixels = list(img.getdata())
    bytes_per_row = (width + 7) // 8
    out = []

    for row in range(height):
        for byte_idx in range(bytes_per_row):
            value = 0
            for bit in range(8):
                col = byte_idx * 8 + bit
                if col >= width:
                    continue
                if pixels[row * width + col]:
                    value |= (0x80 >> bit)
            out.append(value)
    return out

def export_font(font_name, font_file, chars, font_size, output_file):
    font = ImageFont.truetype(str(font_file), font_size)
    chars = list(dict.fromkeys(chars))

    guard = f"{font_name.upper()}_{font_size}_H"
    bytes_per_row = (font_size + 7) // 8

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write("#include <stdint.h>\n\n")

        f.write("#ifndef LCD_FONT_GLYPH_TYPEDEF\n#define LCD_FONT_GLYPH_TYPEDEF\n")
        f.write(
            "typedef struct\n"
            "{\n"
            "    const uint8_t *bitmap;\n"
            "    uint16_t min_col;\n"
            "    uint16_t width;\n"
            "    uint16_t min_row;\n"
            "    uint16_t height;\n"
            "    uint16_t advance;\n"
            "} LCD_FontGlyph;\n\n"
        )
        f.write("#endif\n\n")

        for ch in chars:
            code = ord(ch)

            img = Image.new("1", (font_size, font_size), 0)
            draw = ImageDraw.Draw(img)

            bbox = font.getbbox(ch)
            x0, y0, x1, y1 = bbox

            w = x1 - x0
            h = y1 - y0

            draw_x = ((font_size - w) // 2) - x0
            draw_y = ((font_size - h) // 2) - y0

            draw.text((draw_x, draw_y), ch, font=font, fill=1)

            pixels = list(img.getdata())

            min_col = font_size
            max_col = -1
            min_row = font_size
            max_row = -1

            for row in range(font_size):
                for col in range(font_size):
                    if pixels[row * font_size + col]:
                        min_col = min(min_col, col)
                        max_col = max(max_col, col)
                        min_row = min(min_row, row)
                        max_row = max(max_row, row)

            if max_col < min_col:
                min_col = 0
                width = 0
            else:
                width = max_col - min_col + 1

            if max_row < min_row:
                min_row = 0
                height = 0
            else:
                height = max_row - min_row + 1

            bitmap = bitmap_to_bytes(img)

            bitmap_name = f"glyph_{font_size}_{code:04X}_bitmap"
            glyph_name = f"glyph_{font_size}_{code:04X}"

            f.write(f"// '{ch}'\n")
            f.write(f"static const uint8_t {bitmap_name}[{len(bitmap)}] = {{\n")

            for row in range(font_size):
                start = row * bytes_per_row
                line = bitmap[start:start + bytes_per_row]

                f.write("    ")
                f.write(", ".join(f"0x{x:02X}" for x in line))
                f.write(",\n")

            f.write("};\n\n")

            advance = int(round(font.getlength(ch)))

            f.write(
                f"static const LCD_FontGlyph {glyph_name} =\n"
                "{\n"
                f"    {bitmap_name},\n"
                f"    {min_col},\n"
                f"    {width},\n"
                f"    {min_row},\n"
                f"    {height},\n"
                f"    {advance}\n"
                "};\n\n"
            )

        f.write("#endif\n")

def export_index(font_name, chars, sizes, output_file):
    chars = list(dict.fromkeys(chars))

    with open(output_file, "w", encoding="utf-8") as f:
        guard = f"{font_name.upper()}_INDEX_H"

        f.write(f"#ifndef {guard}\n#define {guard}\n\n")

        for size in sizes:
            f.write(f'#include "{font_name}_{size}.h"\n')

        f.write("\n")

        f.write(
            "static inline const LCD_FontGlyph* "
            f"{font_name}_FindGlyph(char ch, uint16_t size)\n"
            "{\n"
            "    switch(size)\n"
            "    {\n"
        )

        for size in sizes:
            f.write(f"        case {size}:\n")
            f.write("            switch(ch)\n            {\n")

            for ch in chars:
                code = ord(ch)
                esc = ch.replace("\\", "\\\\").replace("'", "\\'")

                f.write(
                    f"                case '{esc}': return &glyph_{size}_{code:04X};\n"
                )

            f.write(
                "                default: return 0;\n"
                "            }\n"
            )

        f.write(
            "        default: return 0;\n"
            "    }\n"
            "}\n\n"
            "#endif\n"
        )

def main():
    project_root = Path(__file__).resolve().parents[1]

    font_dir = project_root / "User" / "GUI" / "Font"

    ttf = font_dir / "Blender.otf"

    if not ttf.exists():
        raise FileNotFoundError(ttf)

    for size in FONT_SIZES:
        export_font(
            FONT_NAME,
            ttf,
            FONT_CHARS,
            size,
            font_dir / f"{FONT_NAME}_{size}.h"
        )

    export_index(
        FONT_NAME,
        FONT_CHARS,
        FONT_SIZES,
        font_dir / f"{FONT_NAME}_Index.h"
    )

    print("Done")

if __name__ == "__main__":
    main()
