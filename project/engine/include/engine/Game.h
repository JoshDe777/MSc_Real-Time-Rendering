#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <box2d/box2d.h>
#include <soloud.h>
#include <soloud_wav.h>
#include <OpenGL/OpenGlInclude.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "engine/Utilities.h"
#include "engine/Systems.h"
#include "engine/Context.h"
#include "engine/ecs/EntityManager.h"

namespace EisEngine {
    using event_t = events::Event<Game, Game&>;
    using ecs::ComponentManager;
    using ecs::EntityManager;
    using components::Transform;
    using ctx::Context;

    /// \n The central class for games created using Eis-Engine.
    /// \n Runs the game loop and holds references to all systems required for running a game.
    class Game {
    public:
        /// \n Creates an instance of a game.
        /// @param title - game window title.
        Game(const std::string &title);
        /// \n Terminates the instance of the game.
        virtual ~Game();

        /// \n Runs the game life until demanded to terminate.
        virtual void run();
        /// \n Signals the engine to close the game window.
        void Quit();

        /// \n Fetches the game window.
        /// @return @a GLFWwindow* - a pointer to a GLFW window.
        [[nodiscard]] GLFWwindow *getWindow();

        /// \n an event invoked right at the start of the game's lifetime.
        event_t onStartup;
        /// \n an event invoked right before the first iteration of the game loop.
        event_t onAfterStartup;
        /// \n an event invoked at the beginning of every frame.
        event_t onBeforeUpdate;
        /// \n an event invoked in the middle of every frame.
        event_t onUpdate;
        /// \n an event invoked at the end of every frame.
        event_t onAfterUpdate;
        /// \n an event invoked right after the game has been asked to terminate.
        event_t onBeforeShutdown;
        /// \n an event invoked right before fully shutting down the game.
        event_t onShutdown;
        /// \n an event invoked every frame when a behaviour is instantiated.
        event_t onEntityStart;

        /// \n the game's component manager.
        ComponentManager componentManager;
        /// \n the game's entity manager.
        EntityManager entityManager;
        /// \n The game context. Gives information about the window / software side of the game.
        Context context;
        /// \n the main camera rendering the scene.
        Camera camera;
        /// \n The system running game physics.
        PhysicsSystem physics;
        /// \n The system synchronizing transforms and rigidbodies.
        PhysicsUpdater physicsUpdater;

        /// \n a transform component determining the game's origin point (0, 0, 0).
        Transform *origin = nullptr;
    protected:
        /// \n Utility function called every frame.
        virtual void update(GLFWwindow *window);
        /// \n Utility function called once at the start of the game's lifetime.
        virtual void start() {}
        /// \n Called every frame. Defines conditions for game termination.
        void CheckForCloseWindowSignal();
        /// \n The game loop, defines the sequence of actions.
        void GameLoop();

        /// \n A system used to draw lines.
        RenderingSystem renderingSystem;
        /// \n A system tasked with managing transform relations.
        SceneGraphPruner sceneGraphPruner;
        /// \n A system tasked with updating transforms.
        SceneGraphUpdater sceneGraphUpdater;
    private:
        /// \n The EisEngine input manager.
        Input input;
        /// \n The EisEngine time manager.
        Time time;
    };
}
