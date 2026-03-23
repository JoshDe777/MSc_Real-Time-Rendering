#include "engine/Game.h"
#include "engine/Systems.h"

namespace EisEngine {
    using ctx::Context;
    using ecs::ComponentManager;
    using ecs::EntityManager;

    Game::Game(const std::string &title): time(*this)
    {
        context = make_unique<Context>(title);
        componentManager = make_unique<ComponentManager>(*this);
        entityManager = make_unique<EntityManager>(*componentManager, *this);
        camera = make_unique<Camera>(*this, context->GetWindowSize());
        renderingSystem = make_unique<RenderingSystem>(*this);
        sceneGraphPruner = make_unique<SceneGraphPruner>(*this);
        sceneGraphUpdater = make_unique<SceneGraphUpdater>(*this);
        physics = make_unique<PhysicsSystem>(*this);
        physicsUpdater = make_unique<PhysicsUpdater>(*this);
        input = make_unique<Input>(*this);
    }

    void Game::GameLoop() {
        onEntityStart.invoke(*this);
        onEntityStart.reset();
        onBeforeUpdate.invoke(*this);
        GLFWwindow *window = context->getWindow();
        update(window);
        onUpdate.invoke(*this);
        onAfterUpdate.invoke(*this);
    }

    void Game::run() {
        onStartup.invoke(*this);
        start();
        glEnable(GL_DEPTH_TEST);
        onAfterStartup.invoke(*this);
        context->run([&](Context &ctx){GameLoop();});

        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    void Game::Quit() { context->CloseWindow();}

    void Game::CheckForCloseWindowSignal() {
        // Alt + F4
        if((Input::GetKeyDown(KeyCode::LeftAlt) ||
        Input::GetKeyDown(KeyCode::RightAlt)) &&
        Input::GetKeyDown(KeyCode::F4))
            context->CloseWindow();
    }

    void Game::update(GLFWwindow *_) { CheckForCloseWindowSignal();}

    Game::~Game() {
        DEBUG_INFO("Closing Game Cleanly.")

        // defined explicitly to find where breaks & errors happen :D
        input.reset();
        physicsUpdater.reset();
        physics.reset();
        sceneGraphUpdater.reset();
        sceneGraphPruner.reset();
        renderingSystem.reset();
        camera.reset();
        entityManager.reset();
        componentManager.reset();
        ResourceManager::Clear();
    }

    GLFWwindow *Game::getWindow() { return context->getWindow();}
}