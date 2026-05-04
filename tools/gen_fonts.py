
#!/usr/bin/env python3
"""
RotomOS — Generador de fonts para LVGL 8
Genera Montserrat con Latin Extended (tildes, ñ, ¿, ¡) en UTF-8 limpio.

Requisitos:
  - Node.js instalado y en el PATH
  - npm install -g lv_font_conv
  - assets/fonts/Montserrat-SemiBold.ttf

Uso: python tools/gen_fonts.py
"""
import subprocess, sys, os, re

FONT_PATH = "assets/fonts/Montserrat-SemiBold.ttf"
OUT_DIR   = "src/fonts"
# ASCII imprimible + Latin Extended A/B (á é í ó ú ñ ü ¿ ¡ etc.)
RANGE     = "0x20-0x7F,0xC0-0x17E"
SIZES     = [12, 14, 16, 32, 48]

def generate(size: int) -> bool:
    name    = f"lv_font_montserrat_{size}_lat"
    outfile = os.path.join(OUT_DIR, f"{name}.c")

    cmd = [
        "npx", "--yes", "lv_font_conv",
        "--font",        FONT_PATH,
        "--size",        str(size),
        "--bpp",         "4",
        "--range",       RANGE,
        "--format",      "lvgl",
        "--no-compress",          # ← sin RLE: no requiere LV_USE_FONT_COMPRESSED
        "-o",            outfile,
    ]

    r = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        # shell=True necesario en Windows para que npx funcione
        shell=(sys.platform == "win32"),
    )

    if r.returncode != 0:
        print(f"  ERROR ({size}px): {r.stderr.strip()}")
        return False

    # ── Leer con utf-8-sig (elimina BOM si lv_font_conv lo añadió) ──────
    with open(outfile, "r", encoding="utf-8-sig") as f:
        content = f.read()

    # ── Corregir include: cualquier variante → <lvgl.h> ─────────────────
    content = re.sub(r'#include\s+"[^"]*lvgl[^"]*"', '#include <lvgl.h>', content)

    # ── Escribir UTF-8 puro, saltos de línea Unix, sin BOM ───────────────
    with open(outfile, "w", encoding="utf-8", newline="\n") as f:
        f.write(content)

    print(f"  ✓  {size:2d}px  →  {outfile}")
    return True

if __name__ == "__main__":
    # Verificar fuente
    if not os.path.exists(FONT_PATH):
        print(f"\nERROR: No encuentro '{FONT_PATH}'")
        print("Descarga Montserrat-SemiBold.ttf de:")
        print("  https://fonts.google.com/specimen/Montserrat")
        print("y guárdala en assets/fonts/\n")
        sys.exit(1)

    os.makedirs(OUT_DIR, exist_ok=True)
    print(f"\nGenerando {len(SIZES)} fonts en {OUT_DIR}/...\n")

    failed = [s for s in SIZES if not generate(s)]

    if failed:
        print(f"\nFallaron los tamaños: {failed}")
        sys.exit(1)
    else:
        print(f"\n{len(SIZES)} fonts generados correctamente.")
        print("Ahora ejecuta: pio run -e emulator_64bits\n")