from pathlib import Path
import argparse

from PIL import Image


DEFAULT_INPUT = Path(__file__).with_name("logo.png")
DEFAULT_OUTPUT = Path(__file__).resolve().parents[1] / "User" / "GUI" / "logo_image.h"
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 480
MAX_LOGO_WIDTH = 440
MAX_LOGO_HEIGHT = 260
BACKGROUND_COLOR = (0, 0, 0, 255)


def rgb888_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def resize_logo(image: Image.Image, max_width: int, max_height: int) -> Image.Image:
    src_w, src_h = image.size
    scale = min(max_width / src_w, max_height / src_h, 1.0)
    dst_w = max(1, int(round(src_w * scale)))
    dst_h = max(1, int(round(src_h * scale)))
    return image.resize((dst_w, dst_h), Image.Resampling.LANCZOS)


def prepare_image(image: Image.Image) -> Image.Image:
    rgba = image.convert("RGBA")
    background = Image.new("RGBA", rgba.size, BACKGROUND_COLOR)
    return Image.alpha_composite(background, rgba).convert("RGB")


def write_header(output_path: Path, image: Image.Image, symbol: str) -> None:
    width, height = image.size
    guard = f"{symbol.upper()}_HEADER_H"
    macro_prefix = symbol.upper()
    pixels_per_line = 12

    output_path.parent.mkdir(parents=True, exist_ok=True)

    with output_path.open("w", encoding="ascii", newline="\n") as f:
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define {macro_prefix}_W {width}U\n")
        f.write(f"#define {macro_prefix}_H {height}U\n\n")
        f.write(
            f"static const uint16_t {symbol}[{macro_prefix}_W * {macro_prefix}_H] = {{\n"
        )

        line = []
        count = 0
        for y in range(height):
            for x in range(width):
                r, g, b = image.getpixel((x, y))
                line.append(f"0x{rgb888_to_rgb565(r, g, b):04X}")
                count += 1
                if len(line) == pixels_per_line:
                    f.write("    " + ", ".join(line) + ",\n")
                    line = []

        if line:
            f.write("    " + ", ".join(line) + ",\n")

        f.write("};\n\n")
        f.write("#endif\n")

    print(f"Generated {output_path}")
    print(f"Image size: {width}x{height}")
    print(f"Pixel count: {count}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert an image into an RGB565 C header.")
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT, help="Input image path.")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT, help="Output header path.")
    parser.add_argument("--symbol", default="g_logo_image_data", help="C array symbol name.")
    parser.add_argument("--max-width", type=int, default=MAX_LOGO_WIDTH, help="Maximum output width.")
    parser.add_argument("--max-height", type=int, default=MAX_LOGO_HEIGHT, help="Maximum output height.")
    args = parser.parse_args()

    image = prepare_image(Image.open(args.input))
    resized = resize_logo(image, args.max_width, args.max_height)
    write_header(args.output, resized, args.symbol)


if __name__ == "__main__":
    main()
