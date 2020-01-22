#pragma once

#include <cstddef>
#include <cstdint>

namespace libgl {

template <typename T>
struct vec2 {
    T x;
    T y;
};

using fvec2 = vec2<float>;
using ivec2 = vec2<int32_t>;

} // namespace libgl