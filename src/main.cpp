#include <cstdlib>
#include <cstring>
#if 0
#    include "../include/defines.hpp"
#    include "../include/shader.h"

std::vector<float>       vertices;
std::vector<unsigned>    indexes;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#    ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#    endif
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // build and compile our shader program
    mlg::Shader shaderProg("../../src/vert_shader.vert", "../../src/frag_shader.frag");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned), indexes.data(), GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // render
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<int>(g_vertices.size() * sizeof(float)),
            g_vertices.data(),
            GL_DYNAMIC_DRAW
        );
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<int>(g_indexes.size() * sizeof(unsigned)),
            g_indexes.data(),
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        // draw our first triangle
        shaderProg.use();

        glBindVertexArray(VAO);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(0xFFFF);
        glDrawElements(GL_LINE_STRIP, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
        glDisable(GL_PRIMITIVE_RESTART);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// clang-format off
// GLfloat vertices[] = {
//     -1, -1,
//     -1,  1,
//      1,  1,
//      1, -1,
// };
GLfloat vertices[] = {
    // 0.0f, 0.0f,
    // 0.0f, 1.0f,
    // 1.0f, 1.0f,
    // 1.0f, 0.0f,
    // 0.0f, 2.0f,
    // 1.0f, 2.0f,
    // 2.0f, 2.0f,
    // 2.0f, 1.0f,
    // 2.0f, 0.0f
    0.0f, 0.0f,
    1.0f, 0.0f,
    2.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    2.0f, 1.0f,
    0.0f, 2.0f,
    1.0f, 2.0f,
    2.0f, 2.0f
};

// GLuint indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
GLuint indices[] = {0, 1, 3, 4, 6, 7, 7, 1, 1, 2, 4, 5, 7, 8};
// clang-format on

struct return_type_t {
    enum { success = 1, failure = 0 } type;
    std::size_t value;

    operator bool() const noexcept { return failure == type ? false : true; }
};

struct shaders_t {
    char const* const vertex;
    char const* const fragment;
};

// clang-format off
static shaders_t shaders{
    .vertex = R"(
        #version 100
        attribute vec2 a_position;

        uniform highp vec4 u_center;
        uniform highp mat4 u_projection;
        // uniform highp vec2 u_extent;

        void main() {
            // gl_Position = u_center + vec4(u_extent * a_position, 0.0, 0.0);
            gl_Position = u_center + (u_projection * vec4(a_position, 0.0, 1.0));
        }
    )",
    .fragment = R"(
        #version 100
        precision mediump float;
        void main() {
            gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )"};
// clang-format on

static return_type_t compile(GLenum const shader_type, char const* source_code) noexcept {
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

static return_type_t create_shader_program() noexcept {
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

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
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

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        GLint const a_position = glGetAttribLocation(shader_program, "a_position");
        glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(a_position);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        auto const projection_matrix = glm::ortho(0.0f, 2.0f, 0.0f, 2.0f, -1.0f, 1.0f);

        glUseProgram(shader_program);
        glUniform4f(glGetUniformLocation(shader_program, "u_center"), 0.0f, 0.0f, 0.0f, 0.0f);
        // glUniform2f(glGetUniformLocation(shader_program, "u_extent"), scale, scale);
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
            glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);

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