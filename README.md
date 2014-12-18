APG
===

A simple framework for getting started writing an RPG using Modern C++ (C++14), SDL2 and
[tmxparser](https://github.com/andrewrk/tmxparser/) to load maps made in [Tiled](http://www.mapeditor.org/).

The name comes from saying the letters "RPG" out loud; "Arr Pee Gee".

What's Included
---------------

- An easy entry-point with clearly defined functions to override and implement to make your own game.
- C++11/14 unique_ptr wrappers around common SDL2 structs (e.g. SDL_Surface, SDL_Window, SDL_Renderer) which let you
stop having to worry about cleanup and just do it for you (see SXXDL).
- A simple extensible framework for managing classes in an "error state", useful for loading and initialising.