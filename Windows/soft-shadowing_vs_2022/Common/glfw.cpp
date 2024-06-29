#include <stdio.h>

#include "util.h"
#include "glfw.h"

static int glMajorVersion = 0;
static int glMinorVersion = 0;

static void glfw_lib_init()
{
    if (!glfwInit()) {
        exit(1);
    }

    int Major, Minor, Rev;
    glfwGetVersion(&Major, &Minor, &Rev);
}

static void init_glew()
{
    glewExperimental = GL_TRUE;
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        exit(1);
    }
}

GLFWwindow* glfw_init(int major_ver, int minor_ver, int width, int height, bool is_full_screen, const char* title)
{
    glfw_lib_init();
    GLFWmonitor* monitor = is_full_screen ? glfwGetPrimaryMonitor() : NULL;
    GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, NULL);

    if (!window) {
        const char* pDesc = NULL;
        int error_code = glfwGetError(&pDesc);
        exit(1);
    }

    glfwMakeContextCurrent(window);
    
    // Must be done after glfw is initialized!
    init_glew();
    glfwSwapInterval(1);
    return window;
}
