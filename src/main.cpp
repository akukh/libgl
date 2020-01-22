#include "libgl/utility.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <libgl/products.hpp>
#include <libgl/shaders.hpp>
#include <libgl/texture.hpp>

using namespace libgl;

constexpr size_t tile_width  = 512;
constexpr size_t tile_height = 512;
constexpr size_t tiles_count = 4;

// clang-format off
fvec2 vertices[] {
    fvec2{0.0f, 0.0f},
    fvec2{0.0f, 1.0f},
    fvec2{1.0f, 0.0f},
    fvec2{1.0f, 1.0f},
};
GLuint indices[]{0, 1, 2, 2, 1, 3};
// clang-format on

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    GLFWwindow* window = glfwCreateWindow(450, 450, "TEST", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return EXIT_FAILURE;
    }

    if (auto const result = create_shader_program(); result) {
        GLuint const shader_program = result.value;

        GLuint vertex_buffer  = 0;
        GLuint vertex_array   = 0;
        GLuint element_buffer = 0;

        glGenVertexArrays(1, &vertex_array);

        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &element_buffer);

        glBindVertexArray(vertex_array);

        // clang-format off
        position_t const lower_bound{0, 0};
        position_t const upper_bound{2, 2};
        // clang-format on
#ifdef ONE_VBO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        auto&& [vertices, indices] = generate_buffers(lower_bound, upper_bound);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(fvec2), vertices.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int32_t), indices.data(), GL_DYNAMIC_DRAW);
#else
        // tile_t const visible_tiles[4]{{0, 0}, {0, 1}, {1, 0}, {1, 1}};

        glm::mat4 const models[]{glm::translate(glm::mat4(1), glm::vec3{0, 0, 0}),
                                 glm::translate(glm::mat4(1), glm::vec3{0, 1, 0}),
                                 glm::translate(glm::mat4(1), glm::vec3{1, 0, 0}),
                                 glm::translate(glm::mat4(1), glm::vec3{1, 1, 0})};

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
        GLuint textures[4];
        glGenTextures(4, textures);

        color_t texture_color{50, 150, 200};
        for (int32_t i = 0; i < tiles_count; i++) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            uint8_t* const texture_data = (uint8_t*)malloc(tile_width * tile_height * 4);
            generate_texture(texture_data, tile_width, tile_height, texture_color);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
            free(texture_data);

            texture_color = color_t{uint8_t(texture_color.r + 30),
                                    uint8_t(texture_color.g + 20),
                                    uint8_t(texture_color.b + 10)};
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        auto const projection_matrix = glm::ortho(lower_bound.xf(),
                                                  upper_bound.xf(),
                                                  lower_bound.yf(),
                                                  upper_bound.yf(),
                                                  -1.0f,
                                                  1.0f);

        glUseProgram(shader_program);
        glUniform4f(glGetUniformLocation(shader_program, "u_center"), 0.0f, 0.0f, 0.0f, 1.0f);
        // glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_projection"),
        //                    1,
        //                    GL_FALSE,
        //                    glm::value_ptr(projection_matrix));

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            for (int32_t i = 0; i < tiles_count; i++) {
                // glUniform2f(glGetUniformLocation(shader_program, "u_texture_coords"), 0, 0);
                glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_mvp"),
                                   1,
                                   GL_FALSE,
                                   glm::value_ptr(models[i] * projection_matrix));

                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

                GLint const a_position = glGetAttribLocation(shader_program, "a_position");
                glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);
                glEnableVertexAttribArray(a_position);

                GLint const a_texcoord = glGetAttribLocation(shader_program, "a_texcoord");
                glVertexAttribPointer(a_texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);
                glEnableVertexAttribArray(a_texcoord);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
                glBindTexture(GL_TEXTURE_2D, textures[i]);
                glDrawElements(/*GL_TRIANGLE_STRIP*/ GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

                glDisableVertexAttribArray(a_position);
                glDisableVertexAttribArray(a_texcoord);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }


        // while (!glfwWindowShouldClose(window)) {
        //     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //     glClear(GL_COLOR_BUFFER_BIT);

        //     // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        //     // glUseProgram(shader_program);
        //     // glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);
        //     glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, nullptr);

        //     // glBindBuffer(GL_ARRAY_BUFFER, 0);
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        //     // glDisableVertexAttribArray(a_position);

        //     glfwSwapBuffers(window);
        //     glfwPollEvents();
        // }

        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &element_buffer);
        glDeleteProgram(shader_program);
        glfwTerminate();
    } else
        std::cerr << "Oops, something happens." << '\n';
}