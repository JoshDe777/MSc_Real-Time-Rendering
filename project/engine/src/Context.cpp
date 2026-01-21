#include <stdexcept>
#include "engine/Context.h"
#include "engine/utilities/Debug.h"

namespace EisEngine::ctx {
    void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height);}

    void Context::InitializeGLFW() {
        if(!glfwInit())
            DEBUG_RUNTIME_ERROR("GL Error - Failed to initialize GLFW")

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    }

    void Context::createWindow(const std::string &title) {
        auto* monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, title.c_str(),nullptr, nullptr);

        if(window == nullptr)
            DEBUG_RUNTIME_ERROR("GL Error - Failed to create Window.");

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    void Context::LoadGLAD() {
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if(glGetError() != GL_NO_ERROR)
            DEBUG_RUNTIME_ERROR("GL Error - Failed to load GLAD")
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    Context::Context(const std::string &title) {
        InitializeGLFW();
        createWindow(title);
        LoadGLAD();
    }

    void Context::run(const Context::Callback& update) {
        glfwSetTime(1.0 / 60);
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            update(*this);
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }

    Context::~Context() { glfwTerminate();}
}