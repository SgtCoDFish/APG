#ifndef INCLUDE_APG_CORE_APGCOMMON_HPP_
#define INCLUDE_APG_CORE_APGCOMMON_HPP_

#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace APG {

struct Vertex {
	float x;
	float y;

	float c;

	float u;
	float v;
};

static constexpr const int VERTEX_SIZE = 5;

namespace internal {
static constexpr const int DEFAULT_SOUND_HANDLE_COUNT = 256;
static constexpr const int DEFAULT_MUSIC_HANDLE_COUNT = 128;
static constexpr const int DEFAULT_FONT_HANDLE_COUNT = 16;

static constexpr const unsigned int MAX_SUPPORTED_TEXTURES = 32;
}

}

#endif
