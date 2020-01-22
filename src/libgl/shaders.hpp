#pragma once

#include <GLFW/glfw3.h>

#include "error.hpp"

namespace libgl {

struct shaders_t {
    char const* const vertex;
    char const* const fragment;
};

// clang-format off
static constexpr shaders_t shaders{
    .vertex = R"(
        #version 100
        attribute vec2 a_position;
        attribute vec2 a_texcoord;

        varying vec2 v_texcoord;

        uniform highp vec4 u_center;
        uniform highp mat4 u_mvp;

        void main() {
            v_texcoord  = a_texcoord;
            gl_Position = u_center + (u_mvp * vec4(a_position, 0.0, 1.0));
        }
    )",
    .fragment = R"(
        #version 100
        precision mediump float;

        varying vec2 v_texcoord;
        uniform sampler2D u_texture_sampler;

        void main() {
            gl_FragColor = texture2D(u_texture_sampler, v_texcoord); // vec4(1.0, 0.5, 0.2, 1.0);
        }
    )"};
// clang-format on

return_type_t compile(GLenum const shader_type, char const* source_code) noexcept;

return_type_t create_shader_program() noexcept;

} // namespace libgl