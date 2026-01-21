#include "engine/systems/ContactListener.h"
#include "engine/Utilities.h"
#include "engine/ecs/Entity.h"
#include "engine/components/BoxCollider2D.h"
#include "engine/Game.h"

namespace EisEngine{
    using namespace ecs;
    using namespace components;

    namespace physics {
        void ContactListener::BeginContact(b2Contact *contact) {
            if(!engine)
                DEBUG_RUNTIME_ERROR("Contact Listener not initialized properly! (missing engine reference)")

            // user data pointers return entity IDs
            auto entityID_A = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
            auto entityID_B = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
            Entity *entityA = nullptr;
            Entity *entityB = nullptr;

            // get entities from IDs
            if(entityID_A != invalidID)
                entityA = engine->entityManager.getEntity(entityID_A);
            if(entityID_A != invalidID)
                entityB = engine->entityManager.getEntity(entityID_B);

            // get colliders and call collision enter methods.
            if(entityA != nullptr && entityB != nullptr){
                auto* colliderA = entityA->GetComponent<BoxCollider2D>();
                auto* colliderB = entityB->GetComponent<BoxCollider2D>();
                colliderA->EnterCollision(*colliderB);
                colliderB->EnterCollision(*colliderA);
            }
        }

        void ContactListener::EndContact(b2Contact *contact) {
            if(!engine)
                DEBUG_RUNTIME_ERROR("Contact Listener not initialized properly! (missing engine reference)")

            // user data pointers return entity IDs
            auto entityID_A = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
            auto entityID_B = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
            Entity *entityA = nullptr;
            Entity *entityB = nullptr;

            // get entities from ids
            if(entityID_A != invalidID)
                entityA = engine->entityManager.getEntity(entityID_A);
            if(entityID_A != invalidID)
                entityB = engine->entityManager.getEntity(entityID_B);

            // get collider components and call collision exit methods.
            if(entityA != nullptr && entityB != nullptr){
                auto* colliderA = entityA->GetComponent<BoxCollider2D>();
                auto* colliderB = entityB->GetComponent<BoxCollider2D>();
                colliderA->ExitCollision(*colliderB);
                colliderB->ExitCollision(*colliderA);
            }
        }
    }
}