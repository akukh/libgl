#pragma once

#include <vector>

#include "utility.hpp"

namespace libgl {

struct position_t {
    int32_t x;
    int32_t y;

    constexpr float xf() const noexcept { return static_cast<float>(x); }
    constexpr float yf() const noexcept { return static_cast<float>(y); }

    constexpr int32_t xi() const noexcept { return x; }
    constexpr int32_t yi() const noexcept { return y; }
};

struct tile_t {
    ivec2 coordinates;
};

using vertex_buffer_t = std::vector<fvec2>;
using index_buffer_t  = std::vector<uint32_t>;
using buffers_t       = std::pair<vertex_buffer_t, index_buffer_t>;

vertex_buffer_t generate_vertex_buffer(ivec2 const& tile_coordinates) {
    ivec2 const lower_bound{tile_coordinates.x, tile_coordinates.y};
    ivec2 const upper_bound{tile_coordinates.x + 1, tile_coordinates.y + 1};

    vertex_buffer_t vertices;
    for (int32_t y = lower_bound.y, v = 0; y <= upper_bound.y; y++, v++) {
        for (int32_t x = lower_bound.x, u = 0; x <= upper_bound.x; x++, u++) {
            vertices.emplace_back(fvec2{float(x), float(y)});
            vertices.emplace_back(fvec2{float(u), float(v)});
        }
    }
    return std::move(vertices);
}

index_buffer_t generate_index_buffer() { return index_buffer_t{0, 1, 2, 2, 1, 3}; }

buffers_t generate_buffers(position_t const& lower_bound, position_t const& upper_bound) {
    size_t const width  = upper_bound.x - lower_bound.x;
    size_t const height = upper_bound.y - lower_bound.y;

    vertex_buffer_t vertices;
    for (int32_t y = lower_bound.y; y <= upper_bound.y; y++) {
        for (int32_t x = lower_bound.x; x <= upper_bound.x; x++) {
            vertices.push_back(fvec2{float(x), float(y)});
        }
    }

    index_buffer_t indices;
    for (uint32_t i = 0; i < width; i++) {
        indices.push_back(i);
        for (uint32_t j = 0; j <= height; j++) {
            indices.push_back(j * (height + 1) + i);
            indices.push_back(j * (height + 1) + 1 + i);
        }
        indices.push_back((width + 1) * width + (i + 1));
    }
    return {std::move(vertices), std::move(indices)};
}


} // namespace libgl