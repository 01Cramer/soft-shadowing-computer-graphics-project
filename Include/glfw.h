#ifndef GLFW_H
#define GLFW_H

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

GLFWwindow* glfw_init(int major_ver, int minor_ver, int width, int height, bool is_full_screen, const char* title);

#endif
