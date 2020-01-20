#include <cstddef>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <type_traits>
#include <vector>

#include <libgl/shaders.hpp>

using namespace libgl;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


struct position_t {
    int32_t x;
    int32_t y;

    constexpr float xf() const noexcept { return static_cast<float>(x); }
    constexpr float yf() const noexcept { return static_cast<float>(y); }

    constexpr int32_t xi() const noexcept { return x; }
    constexpr int32_t yi() const noexcept { return y; }
};

using buffers_t = std::pair<std::vector<float>, std::vector<uint32_t>>;

static buffers_t generate_buffers(position_t const& lower_bound, position_t const& upper_bound) {
    size_t const width  = upper_bound.x - lower_bound.x;
    size_t const height = upper_bound.y - lower_bound.y;

    std::vector<float> vertices;
    for (int32_t y = lower_bound.y; y <= upper_bound.y; y++) {
        for (int32_t x = lower_bound.x; x <= upper_bound.x; x++) {
            vertices.push_back(x);
            vertices.push_back(y);
        }
    }

    std::vector<uint32_t> indices;
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

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        // clang-format off
        position_t const lower_bound{0, 0};
        position_t const upper_bound{2, 2};
        // clang-format on

        auto&& [vertices, indices] = generate_buffers(lower_bound, upper_bound);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

        GLint const a_position = glGetAttribLocation(shader_program, "a_position");
        glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(a_position);

        GLint const a_texcoord = glGetAttribLocation(shader_program, "a_texcoord");
        glVertexAttribPointer(a_texcoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(a_texcoord);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        GLuint texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        constexpr size_t width  = 512;
        constexpr size_t height = 512;

        // clang-format off
        uint8_t* texdata = (uint8_t*)malloc(width * height * 4);
        generate_texture(texdata, width, height, {153, 51, 255});
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raster_size.width(), raster_size.height(), 0,
                                       GL_RGBA, GL_UNSIGNED_BYTE, texdata);
        free(texdata);
        // clang-format on

        auto const projection_matrix = glm::ortho(lower_bound.xf(),
                                                  upper_bound.xf(),
                                                  lower_bound.yf(),
                                                  upper_bound.yf(),
                                                  -1.0f,
                                                  1.0f);

        glUseProgram(shader_program);
        glUniform4f(glGetUniformLocation(shader_program, "u_center"), 0.0f, 0.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_projection"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(projection_matrix));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

            // glUseProgram(shader_program);
            // glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);
            glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, nullptr);

            // glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            // glDisableVertexAttribArray(a_position);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &element_buffer);
        glDeleteProgram(shader_program);
        glfwTerminate();
    } else
        std::cerr << "Oops, something happens." << '\n';
}