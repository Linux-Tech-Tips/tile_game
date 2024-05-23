# Terminal Emulator Tile Reduction Game System

This software, called the Terminal Emulator Tile Reduction Game System (otherwise known as TETRGS), is a game based on the ideas of the popular video game series Tetris. The game field is a grid, with blocks constructed of Tiles, falling downwards, in which the player's task is to Reduce lines of fallen Tiles to gain score.

The gameplay mechanics draw inspiration from the NES version of Tetris, therefore, TETRGS may exclude some modern features players could be used to from newer versions. This is to pay homage to the original game this version draws inspiration from, totally not because I simply didn't feel like implementing the features (lol).

The game is developed in pure C, using only the Linux C standard library, and is developed and tested on Linux terminals.

The game is still work in progress, and while a working demo is available, everything is still subject to change.

To try out the demo, clone the repository on a Linux machine with *gcc* and *make* installed, and execute *make run* in a sufficiently large terminal.

## Development Roadmap

**Features**:
- Better + dynamic GUI, including dialogs instead of key presses
- Code refactoring
- Line clear animations

**Fixes**:
- TBD