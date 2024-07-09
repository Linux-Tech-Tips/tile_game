# Terminal Tile Game

This software, called the Terminal Tile Game, is a game based on the ideas of many previously popular arcade games. 
The game field is a grid, with blocks constructed of Tiles, falling downwards, in which the player's task is to reduce lines of fallen Tiles to gain score.

The gameplay mechanics draw inspiration from older arcade-style games, therefore, the game may exclude some modern features players could be used to. 
This is to pay homage to the older games and their mechanics this version draws inspiration from, 
totally not because I simply didn't feel like implementing the features (lol).

The game is developed in pure C, using only the Linux C standard library, and is developed and tested on Linux terminals.

The main point of the project isn't necessarily to make a game, rather it is to demonstrate the ability to create interactive, text-based GUI 
within a Linux terminal, using only the standard library. In practice, this itself might not be all that useful, it is a fun learning experience though. 
As someone who has worked with the Ncurses library in the past, one day, I thought to myself, "This is fun, but what if I did *all* of it myself??" 
which is where the idea for this style of program comes from.

The game is still work in progress, and while a working demo is available, everything is still subject to change.

To try out the demo, clone the repository on a Linux machine with *gcc* and *make* installed, download the 'terminal_gui' submodule using `git submodule init` and `git submodule update` and execute `make run` in a sufficiently large (>=48x24) terminal.

## Development Roadmap

**Features**:
- Better + dynamic GUI, including dialogs instead of key presses
- High score display, updating and saving/loading
- Next block display
- Code refactoring
- Line clear animations

**Fixes**:
- Flicker on some devices possibly due to erasing without buffered render
- Command line option to disable color mode for colorless terminals
- Fix all warnings once release-applicable version done
