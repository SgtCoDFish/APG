# APG
A simple framework for writing primarily 2D games using Modern C++ (C++14), OpenGL, SDL2 and
[Tiled](http://www.mapeditor.org/) maps.

The design is heavily inspired by [LibGDX](https://github.com/libgdx/libgdx/).

## What's Included
- COMING SOON: emscripten support (WASM)
- An easy entry-point with clearly defined functions to implement to make your own cross platform game.
- A SpriteBatch class similar to the one in [LibGDX](https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/g2d/SpriteBatch.java) which aims to minimise draw calls for 2D sprites.
- An SDL2 input manager for the keyboard supporting "isPressed" and "isJustPressed" as required.
- An SDL2\_mixer audio manager for playing a channel of music and multiple channels of sound effects.
- SDL2\_net/Native Socket wrappers for networking including a modified version of [ByteBufferCpp](https://github.com/SgtCoDFish/ByteBufferCpp) for handling data.
- The ability to compile without SDL/GL at all, allowing you to use the rest of the library freely and reduce dependencies/library size if you don't need some components (ideal for, for example, servers which share some data structures with the client).
- C++14 `unique_ptr` wrappers around common SDL2 structs (`SDL_Surface`, `SDL_Window`, etc) which let you
stop having to worry about cleanup.

## How To Use
## Cloning
Most of APG's dependencies are vendored using git submodules, so just cloning the repository won't be enough to get all the code. You can do something like the following:
```bash
# Clone APG as usual, and cd into the directory
g submodule update --init --recursive  # this will fetch all the dependencies at the correct commit
```

### Dependencies for Build
Generally, you'll need a C++ compiler, zlib, OpenGL and the SDL2 libraries (SDL2, SDL2\_image, SDL2\_mixer, SDL2\_net, SDL2\_ttf) installed and nothing else.

#### WASM
Install [emscripten](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html) as shown in the Wiki.

Activate emscripten (`source ./emsdk_env.sh`) then run:
```bash
python $EMSCRIPTEN/embuilder.py build ALL
```
to build SDL2 and OpenGL.

Don't forget to use `emcmake cmake ..` instead of the regular cmake command.

#### macOS
You'll need Homebrew installed along with git (likely through the XCode command line tools).

```bash
brew install cmake sdl2 sdl2_gfx sdl2_image sdl2_mixer sdl2_net sdl2_ttf glew
```

#### Linux Deb-based (Debian, Ubuntu, etc.)
You should be able to install the dependencies you need through your package manager.

```bash
sudo apt install git cmake  # basic build dependencies; you'll also need a compiler (something like build-essential)
# Now install library dependencies which are not vendored
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-net-dev libsdl2-ttf-dev libglew-dev zlib1g-dev
```

#### Windows
Coming soon!

### Building
Clone the repository and navigate to the folder, then:

```
mkdir build && cd build
cmake ..
make -jN # where N is the number of cores you have
```

If you don't care about the tests you can use `cmake -DEXCLUDE_TESTS=ON`. By default, all tests are built as well as the libraries.

## Usage Notes - Logging
The library depends on easylogging++ which is included by source. To use the library, you simply need to: `#include "easylogging++.h"` in your files. *Important:* In your file which contains your main function you must include the line `INITIALIZE_EASYLOGGINGPP` directly after you include the function.

It's also recommended that you use the function `START_EASYLOGGINGPP(argc, argv);` on your first line of your main function.

For more information on easylogging++, visit [the GitHub repository.](https://github.com/easylogging/easyloggingpp)
