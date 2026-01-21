#include "engine/systems/PhysicsSystem.h"
#include "engine/Game.h"
#include "engine/Components.h"
#include "engine/utilities/Quaternion.h"

namespace EisEngine::systems {
    // static var definitions
    std::vector<b2Body*> PhysicsSystem::bodiesToDelete = {};
    const Vector2 PhysicsSystem::EarthGravityVector = Vector2{0, -9.81f};
    Game* PhysicsSystem::engine = nullptr;

// helper functions:

    // determines whether the target vector is inside a 2d-box drawn between the four provided vectors.
    bool IsVectorWithinBounds(
            const Vector2& target,
            const Vector2& v1,
            const Vector2& v2,
            const Vector2& v3,
            const Vector2& v4) {
        // target in bounds => all cross products are of the same sign;
        auto edge1 = v2 - v1;
        auto v1ToTarget = target - v1;
        auto cross1 = Vector2::CrossProduct(edge1, v1ToTarget);

        auto edge2 = v3 - v2;
        auto v2ToTarget = target - v2;
        auto cross2 = Vector2::CrossProduct(edge2, v2ToTarget);

        if((cross1 > 0 && cross2 < 0) || (cross1 < 0 && cross2 > 0))
            return false;

        auto edge3 = v4 - v3;
        auto v3ToTarget = target - v3;
        auto cross3 = Vector2::CrossProduct(edge3, v3ToTarget);

        // if reached here, cross1 and cross2 fit, so only check cross3
        if((cross1 > 0 && cross3 < 0) || (cross1 < 0 && cross3 > 0))
            return false;

        auto edge4 = v1 - v4;
        auto v4ToTarget = target - v4;
        auto cross4 = Vector2::CrossProduct(edge4, v4ToTarget);

        // if reached here, cross1, cross2 and cross3 fit, so only check cross4
        return (cross1 > 0 && cross4 > 0) ||
        (cross1 < 0 && cross4 < 0);
    }

// physics system methods:

    PhysicsSystem::PhysicsSystem(EisEngine::Game &game, EisEngine::Vector2 _gravity) :
    System(game), gravity(_gravity) {
        if(!engine)
            engine = &game;

        game.onAfterUpdate.addListener([&] (Game &game){ Step();});

        physicsWorld.SetContactListener(&contactListenerInstance);
        contactListenerInstance.engine = &game;
    }

    void PhysicsSystem::SetGlobalGravity(EisEngine::Vector2 newGravity) {
        gravity = newGravity;
        physicsWorld.SetGravity(gravity);
    }

    b2Body *PhysicsSystem::CreateBody(b2BodyDef &bodyDef) { return physicsWorld.CreateBody(&bodyDef);}

    void PhysicsSystem::Step() {
        // do physics update
        physicsWorld.Step( Time::deltaTime, velocityIterations, positionIterations);

        if(bodiesToDelete.empty())
            return;

        // empty the trash.
        for(auto& body : bodiesToDelete){
            body->GetWorld()->DestroyBody(body);
            body = nullptr;
        }
        bodiesToDelete.clear();
    }

    void PhysicsSystem::DeleteBody(b2Body *body) { bodiesToDelete.push_back(body);}

    std::vector<PhysicsBody2D*> PhysicsSystem::FindBodiesWithinBounds(const Vector2& v1,
                                                                      const Vector2& v2,
                                                                      const Vector2& v3,
                                                                      const Vector2& v4) {
        std::vector<PhysicsBody2D*> bodiesInRange = {};
        engine->componentManager.forEachComponent<PhysicsBody2D>([&] (PhysicsBody2D& body){
            if(IsVectorWithinBounds(body.entity()->transform->GetGlobalPosition(), v1, v2, v3, v4))
                bodiesInRange.emplace_back(&body);
        });
        return bodiesInRange;
    }
}
