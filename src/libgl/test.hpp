#pragma once

namespace libgl {

using vertex_buffer_t = std::vector<float>;

struct tile_coordinates_t {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct tile_t {
    uint8_t*           texture;
    tile_coordinates_t coordinates;
};

static vertex_buffer_t generate_vertex_buffer(tile_coordinates_t const& tile_coordinates) noexcept {
    // clang-format off
    ivec2 const lower_bound{tile_coordinates.x,     tile_coordinates.y    };
    ivec2 const upper_bound{tile_coordinates.x + 1, tile_coordinates.y + 1};
    // clang-format on

    vertex_buffer_t vertices;
    for (int32_t y = lower_bound.y, v = 0; y <= upper_bound.y; y++, v++) {
        for (int32_t x = lower_bound.x, u = 0; x <= upper_bound.x; x++, u++) {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }
    return std::move(vertices);
}

} // namespace libgl