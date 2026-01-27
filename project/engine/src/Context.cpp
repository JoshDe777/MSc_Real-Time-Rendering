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

#pragma region GL Error Handling
    inline std::string GetSource(GLenum source)
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
            case GL_DEBUG_SOURCE_OTHER:           return "Other";
            default:                              return "Unknown";
        }
    }

    inline std::string GetType(GLenum type)
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined";
            case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
            case GL_DEBUG_TYPE_MARKER:              return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
            case GL_DEBUG_TYPE_OTHER:               return "Other";
            default:                                return "Unknown";
        }
    }

    inline std::string GetSeverity(GLenum severity)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM:       return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW:          return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            default:                             return "Unknown";
        }
    }

    void APIENTRY GLDebugCallback(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam
    ){
        if (type != GL_DEBUG_TYPE_ERROR)
            return;

        std::string err_message = "[OpenGL Error]\nSource: " + GetSource(source) +
                                  ".\nType: " + GetType(type) +
                                  ".\nSeverity: " + GetSeverity(severity) +
                                  "\nError ID: " + std::to_string(id) +
                                  "\nMessage: " + std::string(message);
        DEBUG_ERROR(err_message)
    }
#pragma endregion

    Context::Context(const std::string &title) {
        InitializeGLFW();
        createWindow(title);
        LoadGLAD();
        LoadImGUI();
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLDebugCallback, nullptr);
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

    Context::~Context() {
        glfwTerminate();
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}