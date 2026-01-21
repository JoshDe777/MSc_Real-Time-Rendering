#include "engine/Game.h"
#include "engine/Systems.h"

namespace EisEngine {
    using ctx::Context;
    using ecs::ComponentManager;
    using ecs::EntityManager;

    Game::Game(const std::string &title): context(title), componentManager(*this),
    entityManager(componentManager, *this), camera(*this, context.GetWindowSize()),
    renderingSystem(*this), sceneGraphPruner(*this), sceneGraphUpdater(*this), physics(*this),
    physicsUpdater(*this), input(*this), time(*this)
    { origin = entityManager.createEntity("origin").transform;}

    void Game::GameLoop() {
        onEntityStart.invoke(*this);
        onEntityStart.reset();
        onBeforeUpdate.invoke(*this);
        GLFWwindow *window = context.getWindow();
        update(window);
        onUpdate.invoke(*this);
        onAfterUpdate.invoke(*this);
    }

    void Game::run() {
        onStartup.invoke(*this);
        start();
        glEnable(GL_DEPTH_TEST);
        onAfterStartup.invoke(*this);
        context.run([&](Context &ctx){GameLoop();});

        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    void Game::Quit() { context.CloseWindow();}

    void Game::CheckForCloseWindowSignal() {
        if((Input::GetKeyDown(KeyCode::LeftAlt) ||
        Input::GetKeyDown(KeyCode::RightAlt)) &&
        Input::GetKeyDown(KeyCode::F4))
            context.CloseWindow();
    }

    void Game::update(GLFWwindow *_) { CheckForCloseWindowSignal();}

    Game::~Game() {
        context.~Context();
        ResourceManager::Clear();
    }

    GLFWwindow *Game::getWindow() { return context.getWindow();}
}