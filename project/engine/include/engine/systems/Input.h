#pragma once

#include "engine/utilities/KeyCode.h"
#include "engine/ecs/System.h"
#include "engine/utilities/Vector2.h"

namespace EisEngine {
    using System = ecs::System;

    class Input: public System {
        friend Game;
    public:
        /// \n Get whether the given keyboard key is pressed in the current frame.
        /// @return true if the given button is being pressed, false otherwise.
        static bool GetKeyDown(KeyCode key);

        /// \n Get whether the left mouse button (LMB) is pressed in the current frame.
        /// @return true if the LMB is being pressed, false otherwise.
        static bool GetLeftMouseButtonDown();

        /// \n Get whether the right mouse button (RMB) is pressed on the current frame.
        /// @return true if RMB is being pressed, false otherwise.
        static bool GetRightMouseButtonDown();

        /// \n The mouse's current position on screen.
        static Vector2 MousePos() {return _mousePos;}

        /// \n The change in mouse position between the current & previous frame.
        static Vector2 MouseDelta() {return _mouseDelta;}

        /// \n The mouse scrolling rate. \n val > 0 = scroll up, val = 0 = not scrolling, val < 0 = scroll down.
        static float MouseScroll() {return _mouseScroll;}
    private:
        /// \n Gets mouse position on screen & the change in position compared to last frame.
        static void MouseCallback();

        /// \n Gets how the mouse is being scrolled if at all:
        static void ScrollCallback(GLFWwindow* window, double x, double y);

        /// \n Creates a new input system.
        explicit Input(Game &engine);

        /// \n A pointer to the active window.
        static GLFWwindow *window;

        /// \n The mouse's current position on screen.
        static Vector2 _mousePos;

        /// \n The change in mouse position between the current & previous frame.
        static Vector2 _mouseDelta;

        /// \n The mouse scrolling rate. \n val > 0 = scroll up, val = 0 = not scrolling, val < 0 = scroll down.
        static float _mouseScroll;
    };
}
