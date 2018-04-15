#ifndef APG_NO_GL

#include <string>

#include "APG/GL.hpp"
#include "APG/graphics/GLError.hpp"

namespace {
constexpr const char *NO_GL_ERROR = "No error";
constexpr const char *INVALID_ENUM {"Invalid value for enum"};
constexpr const char *INVALID_VALUE {"Invalid numeric value"};
constexpr const char *INVALID_OPERATION {"Invalid operation"};
constexpr const char *INVALID_FRAMEBUFFER_OPERATION {"Invalid framebuffer operation"};
constexpr const char *OUT_OF_MEMORY {"Out of memory"};
constexpr const char *UNKNOWN {"Unknown OpenGL Error"};
}

namespace APG {

std::string prettyGLError(GLenum value) {
	switch(value) {
	case GL_NO_ERROR:
		return NO_GL_ERROR;

	case GL_INVALID_ENUM: 
		return INVALID_ENUM;
	
	case GL_INVALID_VALUE:
		return INVALID_VALUE;
	
	case GL_INVALID_OPERATION:
		return INVALID_OPERATION;

	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return INVALID_FRAMEBUFFER_OPERATION;
	
	case GL_OUT_OF_MEMORY:
		return OUT_OF_MEMORY;
	
	default:
		return UNKNOWN;
	}
}

}

#endif
