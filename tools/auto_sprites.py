Import("env")
import subprocess, sys
result = subprocess.run(
    [sys.executable, "tools/png_to_lvgl.py", "assets/sprites/", "src/sprites/"],
    capture_output=True, text=True
)
if result.stdout: print(result.stdout)