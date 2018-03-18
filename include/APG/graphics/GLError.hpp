#ifndef INCLUDE_APG_GRAPHICS_GLERROR_HPP_
#define INCLUDE_APG_GRAPHICS_GLERROR_HPP_

#ifndef APG_NO_GL

#include <string>

#include "APG/GL.hpp"

namespace APG {

std::string prettyGLError(GLenum);

}

#endif

#endif
