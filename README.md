# Programming in Engineering: Final Assignment

The final assignment for Programming in Engineering is a game called Space Debris Evaders. The goal of the game is to prevent you from colliding into other objects in space! The actual goal was to learn C++11 on a basic level.

## Building instructions

The project has two parts: the back-end and the front-end. The back-end is not dependent on any external libraries not part of C++11. The front-end uses OpenGL and related libraries for displaying things.

To demonstrate our knowledge of C++ the back-end is most important. There the useage of classes, inhertiance, etc is shown. The OpenGL front-end really makes the game great, but also makes building the project much more complicated.

Normally CMake is used to build the project. It will configure the compiler to use the correct linking etc, which really reduces the amount of effort for building. If (e.g. for grading) this is unwanted, it should be possible to build the back-end by hand using a simple compiler only. Linking in the OpenGL (and related libraries) is a pain to do by hand, which is why CMake is useful. (Also CMake is the default building environment of our IDE CLion, so that explains why we have it in the first place)

**Building with CMake**

On Linux (should be similar for Windows and other OS):

1. Create a directory for the build files, for example `mkdir build`
2. Enter that directory `cd build`
3. Run CMake on the previous directory `cmake ..`
4. Run make to compile `make all` (add `-j4` with 4 the amount of threads to speed up building)
5. Done! Run `./pie` to execute the game.

**Building by hand**

