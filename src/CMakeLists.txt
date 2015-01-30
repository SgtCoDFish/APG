set (APG_SOURCES
    AnimatedSprite.cpp
    ErrorBase.cpp
    Game.cpp
    Mesh.cpp
    SDLGame.cpp
    ShaderProgram.cpp
    SpriteBatch.cpp
    Sprite.cpp
    SXXDL.cpp
    Texture.cpp
    TmxRenderer.cpp
    VAO.cpp
    VertexAttribute.cpp
    VertexAttributeList.cpp
    VertexBufferObject.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS}")
add_library(APG-d ${APG_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_RELEASE_FLAGS}")
add_library(APG ${APG_SOURCES})

