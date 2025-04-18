## Cross

C++ cross-compilation setup for game development targeting Linux and Windows. The following libraries are supported currently:

- Boost
- FLTK
- GLFW
- GLAD
- Bullet3D
- Assimp

Supports only 64-bit for now. Tested on Arch Linux x86_64. Building natively on linux depends on native built or package manager distributed libraries of the dependencies. For cross-compilation under mingw, the prebuilt libraries are included in `vendor/win64/` except for `Boost` and `FLTK`. They were easily installed from `AUR (mingw-w64-boost and mingw-w64-fltk)`. If `GLAD` doesn't work, refer to `GLAD` documentation to generate `GLAD` sources for your system. If your system doesn't support the provided prebuilt libraries out of the box, you will have to refer to the relevant sources of the dependencies and build them yourself. Future iterations would prefer dependency sources along with manual build procedure instead of pre-compiled libraries.

## Building

Release builds are dependent on `strip` and `upx`. Install them if they're not already installed.

### Native Build

Debug build:

```bash
cmake -S . -B build -DDEBUG_BUILD=ON
cmake --build build
```

Release build:

```bash
cmake -S . -B build -DDEBUG_BUILD=OFF
cmake --build build
```

### Cross-Compilation:

Debug build:

```bash
cmake -S . -B buildwin64 -DCMAKE_TOOLCHAIN_FILE=win64.cmake -DDEBUG_BUILD=ON
cmake --build buildwin64
```

Release build with console disabled:

```bash
cmake -S . -B buildwin64 -DCMAKE_TOOLCHAIN_FILE=win64.cmake -DDEBUG_BUILD=OFF -DWIN_GUI=ON
cmake --build buildwin64
```

