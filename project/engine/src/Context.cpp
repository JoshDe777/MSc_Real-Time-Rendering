#include <stdexcept>
#include <gui/imgui.h>
#include <gui/imgui_impl_glfw.h>
#include <gui/imgui_impl_opengl3.h>

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

    void Context::LoadImGUI() {
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    Context::Context(const std::string &title) {
        InitializeGLFW();
        createWindow(title);
        LoadGLAD();
        LoadImGUI();
    }

    void Context::run(const Context::Callback& update) {
        glfwSetTime(1.0 / 60);
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            update(*this);
            ImGui::EndFrame();
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }

    Context::~Context() {
        glfwTerminate();
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}