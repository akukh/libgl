#define GL_SILENCE_DEPRECATION
#include "shaders.hpp"

#include <iostream>

namespace libgl {

return_type_t compile(GLenum const shader_type, char const* source_code) noexcept {
    GLuint const id = glCreateShader(shader_type);

    GLchar const* source        = reinterpret_cast<GLchar const*>(source_code);
    GLsizei const source_length = static_cast<GLsizei const>(strlen(source_code));

    glShaderSource(id, 1, &source, &source_length);
    glCompileShader(id);

    GLint compile_status = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
    if (GL_FALSE == compile_status) {
        GLchar  message[1024]  = {};
        GLsizei message_length = 0;
        glGetShaderInfoLog(id, sizeof(message), &message_length, message);
        glDeleteShader(id);

        std::cerr << "Cannot compile shader.\n" << message << '\n';
        return return_type_t{.type = return_type_t::failure};
    }

    return return_type_t{.type = return_type_t::success, .value = id};
}

return_type_t create_shader_program() noexcept {
    GLuint const shader_program = glCreateProgram();

    auto const vs = compile(GL_VERTEX_SHADER, shaders.vertex);
    auto const fs = compile(GL_FRAGMENT_SHADER, shaders.fragment);
    if (!vs || !fs) {
        return return_type_t{.type = return_type_t::failure};
    }

    GLuint const vertex_shader   = vs.value;
    GLuint const fragment_shader = fs.value;

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    GLint link_status = GL_FALSE;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_status);
    if (GL_FALSE == link_status) {
        GLchar  message[1024]  = {};
        GLsizei message_length = 0;
        glGetProgramInfoLog(shader_program, sizeof(message), &message_length, message);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(shader_program);

        std::cerr << "Cannot link shader program.\n" << message << '\n';
        return return_type_t{.type = return_type_t::failure};
    }

    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return return_type_t{.type = return_type_t::success, .value = shader_program};
}

} // namespace libgl