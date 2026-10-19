# CHIP-8 Emulator

A small CHIP-8 emulator written in C++ with Raylib. It includes the CHIP-8 CPU, memory, timers, display, keyboard input, and a simple desktop window for running ROMs.

This project is mainly intended as a compact emulator implementation and a way to explore how the CHIP-8 instruction set works.

## Features

- 4 KB of CHIP-8 memory
- 16 general-purpose 8-bit registers
- 16-level call stack
- Delay and sound timers
- 64 x 32 monochrome display
- CHIP-8 hexadecimal keypad input
- Sprite drawing with collision detection and screen wrapping
- Support for the standard CHIP-8 instruction set used by the included games
- ROM loading from the `games` directory

## Requirements

- A C++17-compatible compiler
- CMake 3.20 or newer
- Raylib

On macOS, Raylib can be installed with Homebrew:

```sh
brew install raylib
```

## Build and run

Configure and build the project from the repository root:

```sh
cmake -S . -B build
cmake --build build
```

The current executable expects to be launched from the `build` directory because the default ROM path is relative to that location:

```sh
cd build
./chip8
```

The default ROM is `games/Tic-Tac-Toe [David Winter].ch8`. To run a different ROM, change the path passed to `LoadROM` in `src/main.cpp` and rebuild.

## Controls

The CHIP-8 keypad is mapped to the keyboard as follows:

```text
0 1 2 3    ->    X 1 2 3
4 5 6 7    ->    Q W E A
8 9 A B    ->    S D Z C
C D E F    ->    4 R F V
```

In other words, the emulator uses this keyboard layout:

```text
X 1 2 3
Q W E A
S D Z C
4 R F V
```

## Included ROMs

The repository currently includes:

- `games/Tic-Tac-Toe [David Winter].ch8`
- `games/br8kout.ch8`

## Project layout

```text
src/machine.hpp   CHIP-8 state and instruction declarations
src/machine.cpp   CHIP-8 instruction implementation
src/main.cpp      Raylib window, input handling, and rendering
games/            CHIP-8 ROM files
```

## Notes

This is a learning-focused project and is still being developed. Timing, audio output, and emulator compatibility can vary between ROMs. The current display uses a fixed 20-pixel scale, resulting in a 1280 x 640 window.

## License

No license has been selected for this project yet.
