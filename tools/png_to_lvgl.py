#!/usr/bin/env python3
"""
RotomOS — PNG to LVGL v8 image converter
Genera .c compatibles con CF_TRUE_COLOR_ALPHA sin retoques manuales.

Deps: pip install Pillow
Uso individual:  python tools/png_to_lvgl.py <in.png> <out.c> <var_name>
## python tools/png_to_lvgl.py assets/cosmog.png src/cosmog_sprite.c cosmog_sprite
Uso batch:       python tools/png_to_lvgl.py <sprites_dir/> <out_dir/>
## python tools/png_to_lvgl.py assets/sprites/ src/sprites/
"""
from PIL import Image
import os, sys

def convert(png_path: str, c_path: str, var: str):
    img = Image.open(png_path).convert("RGBA")
    w, h = img.size
    px = list(img.getdata())

    data = []
    for r, g, b, a in px:
        rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        data += [rgb565 & 0xFF, (rgb565 >> 8) & 0xFF, a]

    hex_rows = []
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_rows.append("    " + ", ".join(f"0x{x:02X}" for x in chunk) + ",")

    V = var.upper()
    out = f"""\
#include "lvgl.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_IMG_{V}
#define LV_ATTRIBUTE_IMG_{V}
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_{V} uint8_t {var}_map[] = {{
{chr(10).join(hex_rows)}
}};

const lv_img_dsc_t {var} = {{
    .header.always_zero = 0,
    .header.w           = {w},
    .header.h           = {h},
    .data_size          = {w * h * 3},
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
    .data               = {var}_map,
}};
"""
    os.makedirs(os.path.dirname(c_path) or ".", exist_ok=True)
    with open(c_path, "w") as f:
        f.write(out)
    print(f"✓  {os.path.basename(png_path):30s} → {os.path.basename(c_path)}  ({w}×{h})")

if __name__ == "__main__":
    if len(sys.argv) == 3 and os.path.isdir(sys.argv[1]):
        src_dir, dst_dir = sys.argv[1], sys.argv[2]
        converted = 0
        for fname in sorted(os.listdir(src_dir)):
            if fname.lower().endswith(".png"):
                var = os.path.splitext(fname)[0]
                convert(os.path.join(src_dir, fname),
                        os.path.join(dst_dir, f"{var}.c"), var)
                converted += 1
        print(f"\n{converted} archivo(s) convertido(s).")
    elif len(sys.argv) == 4:
        convert(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
        print(__doc__)
        sys.exit(1)