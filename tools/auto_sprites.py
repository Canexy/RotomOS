Import("env")
import subprocess, sys, os

src_dir = "assets/sprites"
dst_dir = "src/sprites"

if not os.path.isdir(src_dir) or not os.listdir(src_dir):
    print(f"[auto_sprites] Sin sprites en {src_dir}, saltando conversion.")
else:
    result = subprocess.run(
        [sys.executable, "tools/png_to_lvgl.py", src_dir + "/", dst_dir + "/"],
        capture_output=True, text=True
    )
    if result.stdout: print(result.stdout)
    if result.returncode != 0: print(result.stderr)