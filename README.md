APG
===

A simple framework for getting started writing an RPG using Modern C++ (C++14), SDL2 and
[tmxparser](https://github.com/andrewrk/tmxparser/) to load maps made in [Tiled](http://www.mapeditor.org/).

The design is heavily inspired by [LibGDX](https://github.com/libgdx/libgdx/).

The name comes from saying the letters "RPG" out loud; "*A*rr *P*ee *G*ee".

What's Included
---------------

- An easy entry-point with clearly defined functions to override and implement to make your own cross platform game.
- SXXDL: C++14 `unique_ptr` wrappers around common SDL2 structs (e.g. `SDL_Surface`, `SDL_Window`, `SDL_Renderer`) which let you
stop having to worry about cleanup.
- A SpriteBatch class similar to the one in [LibGDX](https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/g2d/SpriteBatch.java) which aims to minimise draw calls for 2D sprites.
- Two renderers for TMX maps; one using pure SDL2 `SDL_Renderer` functions, one using pure modern OpenGL (core context).
- An SDL2 input manager for the keyboard supported "isPressed" and "isJustPressed" as required.

How To Use
----------

Clone the repository and navigate to the folder, then:

```
mkdir build && cd build
cmake ..
make -jN # where N is the number of cores you have
```

If you don't care about the render tests, you can use `cmake -DEXCLUDE_SDL_TEST=TRUE ..` or `cmake -DEXCLUDE_GL_TEST=TRUE ..` or both exclusions. By default, both tests are built as well as the libraries.