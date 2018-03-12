APG
===

A simple framework for writing primarily 2D game using Modern C++ (C++14), OpenGL SDL2 and
[Tiled](http://www.mapeditor.org/) maps.

The design is heavily inspired by [LibGDX](https://github.com/libgdx/libgdx/).

What's Included
---------------

- An easy entry-point with clearly defined functions to override and implement to make your own cross platform game.
- A SpriteBatch class similar to the one in [LibGDX](https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/g2d/SpriteBatch.java) which aims to minimise draw calls for 2D sprites.
- Two renderers for TMX maps; one using pure SDL2 `SDL_Renderer` functions, one using pure modern OpenGL (core context).
- An SDL2 input manager for the keyboard supporting "isPressed" and "isJustPressed" as required.
- An SDL2\_mixer audio manager for playing a channel of music and multiple channels of sound effects.
- SDL2\_net/Native Socket wrappers for networking including a modified version of [ByteBufferCpp](https://github.com/SgtCoDFish/ByteBufferCpp) for handling data.
- The ability to compile without SDL/GL at all, allowing you to use the rest of the library freely and reduce dependencies/library size if you don't need some components (ideal for, for example, servers which share some data structures with the client).
- SXXDL: C++14 `unique_ptr` wrappers around common SDL2 structs (e.g. `SDL_Surface`, `SDL_Window`, `SDL_Renderer`) which let you
stop having to worry about cleanup.

How To Use
----------

## Dependencies for Build
### Linux
Can all be done through a package manager; details coming soon.

### macOS
You'll need Homebrew installed.

```bash
brew install sdl2 sdl2_gfx sdl2_image sdl2_mixer sdl2_net sdl2_ttf glew glm rapidjson tinyxml2
```

### Windows
Coming soon!

Clone the repository and navigate to the folder, then:

```
mkdir build && cd build
cmake ..
make -jN # where N is the number of cores you have
```

If you don't care about the render tests, you can use `cmake -DEXCLUDE_SDL_TEST=ON ..` or `cmake -DEXCLUDE_GL_TEST=ON ..` or both exclusions. By default, both tests are built as well as the libraries.

The audio test is slightly different, and is off by default. You can similarly use `-DEXCLUDE_AUDIO_TEST=OFF` but the option also requires you to place a file, `test_music.ogg` in the base `assets/` directory before running cmake. This is to avoid shipping a test music file which would be a waste of repository space.

Usage Notes - Logging
---------------------

The library depends on easylogging++ which is included by source. To use the library, you simply need to: `#include "easylogging++.h"` in your files. *Important:* In your file which contains your main function you must include the line `INITIALIZE_EASYLOGGINGPP` directly after you include the function.

It's also recommended that you use the function `START_EASYLOGGINGPP(argc, argv);` on your first line of your main function.

For more information on easylogging++, visit [the GitHub repository.](https://github.com/easylogging/easyloggingpp)
