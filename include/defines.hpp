#ifndef DEFINES_HPP
#define DEFINES_HPP

/*
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#define SIZE 1024

const unsigned int WIDTH  = 800;
const unsigned int HEIGHT = 600;

extern std::vector<float>       g_vertices;
extern std::vector<unsigned>    g_indexes;
extern bool                     g_mouseClicked;
extern bool                     g_mouseReleased;
extern float                    g_X1;
extern float                    g_Y1;
extern float                    g_X2;
extern float                    g_Y2;

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

inline void mouseButtonCallback(GLFWwindow* window, int scancode, int action, int mods	)
{
    double		xpos  = .0, ypos  = .0;
    int			width = 0,  height = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);
    if (scancode == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        g_mouseClicked  = true;
        g_mouseReleased = false;

        g_X1 = static_cast<float>( (xpos - (width  / 2)) / (width  / 2));
        g_Y1 = static_cast<float>(-(ypos - (height / 2)) / (height / 2));
        g_vertices.push_back(g_X1);
        g_vertices.push_back(g_Y1);
        g_vertices.push_back(0.f);
    }
    else if (action == GLFW_RELEASE)
    {
        g_mouseClicked  = false;
    }
}
*/
#endif // DEFINES_HPP
