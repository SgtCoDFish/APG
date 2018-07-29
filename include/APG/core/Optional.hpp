#ifndef APG_CORE_OPTIONAL_HPP
#define APG_CORE_OPTIONAL_HPP

#if __cplusplus >= 201703L

#include <optional>

namespace APG {
namespace shim {

template <typename T> using optional = std::optional<T>;
constexpr std::experimental::nullopt_t nullopt {std::experimental::nullopt};

}
}

#else

#include <experimental/optional>

namespace APG {
namespace shim {

template<typename T> using optional = std::experimental::optional<T>;
constexpr std::experimental::nullopt_t nullopt {std::experimental::nullopt};

}
}

#endif

#endif