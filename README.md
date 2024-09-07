# Simple Chip8 Emulator with limited debugging capabilities

Keyboard is mapped as

1 2 3 4 -> 0, 1, 2 ,3
q w e r -> 4, 5, 6, 7
a s d f -> 8, 9, a, b
z x c v -> c, d, e, f

# Usage: 
`./main <rom_file> <debug_mode_on_start>`

# Debugging:
Just put any key after rom file and emulator will start in debug mode.
## Debugging keys:
If pressed i in debug mode it'll print help.<br>

`p -> Resume Program ( Not Available yet)`<br>
`n -> Run Next Instruction`<br>
`s -> Show Stack`<br>
`m -> Show Memory`<br>
`r -> Show Registers`<br>
`v -> Show Video Memory`<br>

Rest should be self explanatory.

# Compiling:
## Requirements:<br>
OS: Tested on MacOS, Linux and Windows support not tested<br>
Lib: SDL2

Place SDL2 libraries under lib folder.

## Command:
Just `make` is enough. Executable will be under bin/ as `main`