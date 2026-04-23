import os
Import("env")

# Forzamos las rutas de MSYS2/Mingw64
mingw_include = "C:/msys64/mingw64/include"
mingw_lib = "C:/msys64/mingw64/lib"

env.Append(
    CPPPATH=[mingw_include],
    LIBPATH=[mingw_lib],
    LIBS=["SDL2", "SDL2main"],
    CCFLAGS=["-I" + mingw_include]
)

# Esto evita errores de "undefined reference" al compilar en Windows
env.Append(LINKFLAGS=["-static-libgcc", "-static-libstdc++"])