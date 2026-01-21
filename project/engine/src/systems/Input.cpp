#include "engine/systems/Input.h"
#include "engine/Game.h"

namespace EisEngine {
    GLFWwindow *Input::window = nullptr;
    Vector2 Input::_mousePos = Vector2();
    Vector2 Input::_mouseDelta = Vector2();
    float Input::_mouseScroll = 0;

    static bool firstCall = true;

    void Input::MouseCallback() {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        auto currentPos = Vector2((float) x, (float) y);
        auto lastPos = _mousePos;

        if(firstCall){
            lastPos = currentPos;
            firstCall = false;
        }

        _mouseDelta = lastPos - currentPos;
        _mousePos = currentPos;
    }

    void Input::ScrollCallback(GLFWwindow *window, double x, double y) {_mouseScroll = (float) y;}

    Input::Input(EisEngine::Game &engine) : System(engine) {
        window = engine.getWindow();
        glfwSetScrollCallback(window, Input::ScrollCallback);
        engine.onBeforeUpdate.addListener([&] (Game& game){
            MouseCallback();
        });
    }

    bool Input::GetKeyDown(EisEngine::KeyCode key) {
        if (window == nullptr) return false;
        return glfwGetKey(window, (int) key) == GLFW_PRESS;
    }

    bool Input::GetLeftMouseButtonDown() {
        if (window == nullptr) return false;
        return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    }

    bool Input::GetRightMouseButtonDown() {
        if (window == nullptr) return false;
        return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    }
}