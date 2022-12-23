Mostly UCI compatible chess engine.

# Limitations

- Primitive evaluation function
- Unresizable 1Gb cache
- Stops searching after 1 second

# Dependencies

- PCRE2 (for parsing)
- SDL2 (for portable threads, atomics, event loop)
- Mimalloc **optional**

All of which can be installed with vcpkg:
`vcpkg install pcre2 sdl2 mimalloc`

# Building

```
mkdir build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=[PATH TO VCPKG TOOLCHAIN] -DVCPKG_TARGET_TRIPLET=[VCPKG TRIPLET] -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
