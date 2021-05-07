# checkers
A checkers game programmed in C that runs in the terminal.

Currently working on a more interactive terminal interface using ncurses.
Also planning to implement a minimax algorithm in the future for the player to play against the computer.

Here's the previous stdin/stdout-based interaction loop.
Plays checkers correctly, but not as usable.
![gif](./checkers-demo.gif)

Here's the current ncurses-based interaction loop.
As you can see, now WASD cycles through the options (W and D to go forward and S and A to go backwards),
but it's still technically wrong since it doesn't allow sequential movements.
![gif](./new-checkers-demo.gif)

