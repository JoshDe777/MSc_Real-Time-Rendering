#include "engine/systems/PhysicsUpdater.h"
#include "engine/components/PhysicsBody2D.h"
#include "engine/Game.h"

namespace EisEngine::systems {
    using Game = EisEngine::Game;
    using PhysicsBody = EisEngine::components::PhysicsBody2D;

    PhysicsUpdater::PhysicsUpdater(Game &engine) : System(engine)
    { engine.onBeforeUpdate.addListener([&] (Game &game) { SyncBodiesToTransforms(game);});}

    void PhysicsUpdater::SyncBodiesToTransforms(Game &engine) {
        if(!engine.componentManager.hasComponentOfType<PhysicsBody>())
            return;
        engine.componentManager.forEachComponent<PhysicsBody>([&](PhysicsBody &body) {body.SyncPhysics();});
    }
}
