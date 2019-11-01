#pragma once


namespace Init
{
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void init_glfw();
    GLFWwindow *init_window(int width, int height);
    void init_glad();

    GLFWwindow *init_all(int width, int height);
}