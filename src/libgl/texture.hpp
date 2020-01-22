#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace libgl {

struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void generate_texture(uint8_t* const texdata, size_t const width, size_t const height, color_t color) noexcept {
    assert(texdata && "must be non-nullptr");

    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            texdata[i * height * 4 + j * 4 + 0] = color.r;
            texdata[i * height * 4 + j * 4 + 1] = color.g;
            texdata[i * height * 4 + j * 4 + 2] = color.b;
            texdata[i * height * 4 + j * 4 + 3] = 255;
        }
    }
}

} // namespace libgl