# SpaceInvaders
My version of the classic space invaders

### Performance based code
This code is specifically written to execute on a low memory, low processor spec machine. 
For this reason, I have used a lot of bit manipulation to do faster calculations. 
Also, a single sprite is represented as a single bit which saves considerable memory


 
### Description 
Space Invaders consists of four different objects: the player ship, the aliens,
the aliens' bombs and the player's rockets. The goal of the game is to shoot as
many aliens as possible without being hit by them or their bombs. 
 
### The player ship 
The player ship is positioned at the bottom of the screen. It can only move
horizontally. It can fire rockets at the aliens. The ship can take three hits
from aliens or their bombs before it is destroyed and the game is over. 
 
### The rockets 
The rockets travel straight up. If they hit an alien, the alien and the rocket
are destroyed, and the player's score is increased. Rockets cannot destroy
bombs. When the rocket reaches the top of the screen, it disappears. 

### The aliens 
The aliens start out at the top left corner of the screen, organized in rows and
columns. They move slowly to the right, until the rightmost alien reaches the
border of the screen. Then, they all take one step down, and start travel to the
left instead. If the alien collides with the player, the alien is destroyed and
the player's health is decreased. The aliens drop bombs randomly. 
 
### The bombs 
The bombs travel straight down. If they hit the player, the player's health is
decreased and the bomb is destroyed. A bomb disappears when it is outside the
bottom of the screen. The bombs don't hurt other aliens. 
 
### The end of the game 
If the player is hit three times by aliens or their bombs, or an alien reaches
the bottom of the screen, the player loses and the game is over. If the player
kills all aliens, a new alien army should be created at the top of the screen. 
 

Compiling the Game (Windows)
----------------------------

### Basics
- Launch a Visual Studio command line environment (with nmake.exe and cl.exe in
  the path)
- CD to the unzipped assignment
- Run `nmake` to build the code
- Run `DiceInvaders.exe`

### Additional Build Targets
- `nmake` - builds a debug version of the game
- `nmake release` - builds an optimized version of the game
- `nmake dev` - build a debug version and opens up the game in the visual studio
  environment.
- `nmake clean` - cleans up the directory from intermediate files


Compiling the Game (Linux and macOS)
------------------------------------
(Only tested on Ubuntu 16.04 LTS and macOS 10.12)

### Basics
- CD to the unzipped assignment
- CD into the "sdl" directory
- Make sure you have SDL2 installed
  - Linux: `sudo apt-get install libsdl2-dev` (depending on distro)
  - macOS: Install the development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php) into `/Library/Frameworks`
- Run `make -f makefile.sdl` to build the code
- Run `./SpaceInvaders`

### Additional Build Targets
- `make -f makefile.sdl` - builds the game
- `make -f makefile.sdl clean` - cleans up the directory from intermediate files


Implementation 
--------------

### Files

`Spacenvaders.cpp`
- Invokes the engine and starts the game

`Engine.h` 
- The library interface. Should be self-documented.

`Engine.cpp` (Do not edit!)
- The implementation of the library.



`makefile`
- Simple makefile for compiling the game. Keep this makefile up to date if you
  add files to the project.

`makefile.sdl`
- Makefile for compiling the game on macOS and Linux with SDL support.



