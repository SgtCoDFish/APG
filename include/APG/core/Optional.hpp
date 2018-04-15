#ifndef APG_CORE_OPTIONAL_HPP
#define APG_CORE_OPTIONAL_HPP

namespace APG {
namespace shim {

#if __cplusplus >= 201703L

#include <optional>
template <typename T> using optional = std::optional<T>;
constexpr std::experimental::nullopt_t nullopt {std::experimental::nullopt};

#else

#include <experimental/optional>

template<typename T> using optional = std::experimental::optional<T>;
constexpr std::experimental::nullopt_t nullopt {std::experimental::nullopt};

#endif

}
}

#endif
