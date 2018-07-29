#ifndef INCLUDE_APG_INTERNAL_ASSERT_HPP_
#define INCLUDE_APG_INTERNAL_ASSERT_HPP_

#ifndef NDEBUG

#include <cstdio>

#include <stdexcept>

#include "spdlog/spdlog.h"

#define REQUIRE(expr, orElse) \
	do { \
if(expr) {} \
else \
{ \
	APG::internal::require_impl(#expr, orElse, __FILE__, __LINE__); \
} \
} while(0)

namespace APG {
namespace internal {

inline void require_impl(const char *expression, const char *extra, const char *filename, int line) {
	spdlog::get("APG")->critical("Assertion failure in \"{}\" (line {}): {} ({})", filename, line, expression,
	        extra);
	throw std::runtime_error("assertion failure");
}

}
}

#else
#define REQUIRE(expr, orElse) /* nothing */
#endif

#endif