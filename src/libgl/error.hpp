#pragma once

#include <cstddef>

namespace libgl {

struct return_type_t {
    // clang-format off
    enum { failure, success } type;
    size_t                    value;
    // clang-format on

    constexpr operator bool() const noexcept { return failure == type ? false : true; }
};

} // namespace libgl