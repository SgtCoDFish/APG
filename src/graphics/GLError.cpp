#ifndef APG_NO_GL

#include <string>

#include "APG/GL.hpp"
#include "APG/graphics/GLError.hpp"

namespace {
static const std::string NO_GL_ERROR {"No error"};
static const std::string INVALID_ENUM {"Invalid value for enum"};
static const std::string INVALID_VALUE {"Invalid numeric value"};
static const std::string INVALID_OPERATION {"Invalid operation"};
static const std::string INVALID_FRAMEBUFFER_OPERATION {"Invalid framebuffer operation"};
static const std::string OUT_OF_MEMORY {"Out of memory"};
static const std::string UNKNOWN {"Unknown OpenGL Error"};
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
