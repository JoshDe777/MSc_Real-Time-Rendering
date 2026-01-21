#include "engine/components/PhysicsBody2D.h"
#include "engine/components/BoxCollider2D.h"
#include "engine/Game.h"

namespace EisEngine::components {
    using EisEngine::Bounds2D;

// helper functions
    // returns the geometric average x and y coordinates, defining the center of mass.
    // Not sure if actually used but it's there
    b2Vec2 CalculateCenterOfMass(const std::vector<b2Vec2> &v){
        float x = 0;
        float y = 0;
        for(auto vertex : v){
            x += vertex.x;
            y = vertex.y;
        }

        return {x/(float) v.size(), y/(float) v.size()};
    }

    // converts a bounds object to a box of b2Vec2's.
    std::vector<b2Vec2> BoundsToB2Vec2(const Bounds2D &bounds){
        return {
            b2Vec2(bounds.left, bounds.top),
            b2Vec2(bounds.left, bounds.bottom),
            b2Vec2(bounds.right, bounds.bottom),
            b2Vec2(bounds.right, bounds.top)
            };
    }

    // takes a vector of b2Vec2 vertices and applies the specified scale to all vertices.
    std::vector<b2Vec2> RescaleVertices(const std::vector<b2Vec2> &v, const Vector3 &scale) {
        std::vector<b2Vec2> vertices = {};
        vertices.reserve(v.size());
        for(auto vertex : v)
            vertices.emplace_back(vertex.x * scale.x, vertex.y * scale.y);
        return vertices;
    }

// PhysicsBody methods:
    PhysicsBody2D::PhysicsBody2D(Game &engine,
                                 guid_t owner,
                                 b2BodyType type) :
    Component(engine, owner),
    rawVertices(entity()->GetComponent<BoxCollider2D>() ?
            BoundsToB2Vec2(*entity()->GetComponent<BoxCollider2D>()->GetBounds()) :
            std::vector<b2Vec2>{b2Vec2(0, 0)}) // "empty" vector of b2Vec2's if no collider found.
    {
        if(!entity()->GetComponent<BoxCollider2D>()) {
            DEBUG_ERROR("No Collider found for physics body assigned to entity " + entity()->name() + ".")
            // would and probably should call Invalidate() here but in constructor I'm getting advice not to do so...
            return;
        }
        transform = entity()->transform;
        scale = transform->GetGlobalScale();
        b2BodyDef bodyDef;
        bodyDef.type = type;
        bodyDef.position = transform->GetGlobalPosition();
        bodyDef.angle = transform->GetLocalRotation().z;
        bodyDef.userData.pointer = owner;
        body = engine.physics.CreateBody(bodyDef);

        b2PolygonShape bodyShape;
        bodyShape.Set(rawVertices.data(),
                      static_cast<int>(rawVertices.size()));

        // box2D error prevention - bodies have at least 3 vertices.
        if(bodyShape.m_count < 3){
            DEBUG_ERROR("Entity " + entity()->name() + "::PhysicsBody: Invalid amount of vertices in shape "
                                                       "defined by vertices: " + to_string(rawVertices))
            return;
        }

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &bodyShape;
        fixtureDef.density = density;
        fixtureDef.friction = friction;
        fixtureDef.restitution = bounciness;
        fixtureDef.isSensor = isSensor;
        body->CreateFixture(&fixtureDef);
        body->SetFixedRotation(true);

        body->SetLinearDamping(1.0f);
        body->SetGravityScale(1.0f);
    }

    void PhysicsBody2D::Invalidate() {
        Component::Invalidate();
        if(!body)
            return;
        PhysicsSystem::DeleteBody(body);
    }

    // getters
    Vector2 PhysicsBody2D::GetVelocity() const {
        if (!body){
            DEBUG_ERROR("Trying to get velocity from invalid Rigidbody!")
            return Vector2::zero;
        }
        return Vector2(body->GetLinearVelocity());
    }
    float PhysicsBody2D::GetFriction() const {
        if(!body){
            DEBUG_ERROR("Trying to get friction value of invalid Rigidbody!")
            return 0;
        }
        return friction;
    }
    float PhysicsBody2D::GetDensity() const {
        if(!body){
            DEBUG_ERROR("Trying to get density value of invalid Rigidbody!")
            return 0;
        }
        return density;
    }
    float PhysicsBody2D::GetBounciness() const {
        if(!body){
            DEBUG_ERROR("Trying to get restitution/bounciness value of invalid Rigidbody!")
            return 0;
        }
        return bounciness;
    }
    bool PhysicsBody2D::IsSensor() const {
        if(!body){
            DEBUG_ERROR("Trying to get sensor status of invalid Rigidbody!")
            return false;
        }
        return isSensor;
    }
    bool PhysicsBody2D::IsKinematic() const {
        if(!body){
            DEBUG_ERROR("Trying to get kinematics of invalid Rigidbody!")
            return true;
        }
        return isKinematic;
    }
    bool PhysicsBody2D::UsesGravity() const {
        if(!body){
            DEBUG_ERROR("Trying to get gravity effect of invalid Rigidbody!")
            return false;
        }
        return usesGravity;
    }

    // setters
    void PhysicsBody2D::SetVelocity(const Vector2& velocity) const {
        if(!body){
            DEBUG_ERROR("Trying to apply velocity to invalid Rigidbody!")
            return;
        }
        body->SetLinearVelocity((b2Vec2) velocity);
    }
    void PhysicsBody2D::SetFriction(float value) {
        if(!body){
            DEBUG_ERROR("Trying to set friction of invalid Rigidbody!")
            return;
        }
        friction = value;
        body->GetFixtureList()->SetFriction(value);
    }
    void PhysicsBody2D::SetDensity(float value) {
        if(!body){
            DEBUG_ERROR("Trying to set density of invalid Rigidbody!")
            return;
        }
        density = value;
        body->GetFixtureList()->SetDensity(value);
    }
    void PhysicsBody2D::SetBounciness(float value) {
        if(!body){
            DEBUG_ERROR("Trying to set bounciness of invalid Rigidbody!")
            return;
        }
        bounciness = value;
        body->GetFixtureList()->SetRestitution(value);
    }
    void PhysicsBody2D::SetSensorStatus(bool value) {
        if(!body){
            DEBUG_ERROR("Trying to set sensor status of invalid Rigidbody!")
            return;
        }
        isSensor = value;
        body->GetFixtureList()->SetSensor(value);
    }
    void PhysicsBody2D::SetGravityUse(bool value) {
        if(!body){
            DEBUG_ERROR("Trying to set gravity use of invalid Rigidbody!")
            return;
        }
        usesGravity = value;
        body->SetGravityScale(value);
    }

    void PhysicsBody2D::IgnoreAllCollisions() const {
        b2Fixture* fixture = body->GetFixtureList();
        while (fixture) {
            b2Filter filter = fixture->GetFilterData();
            filter.maskBits = 0;  // Ignore all collisions
            fixture->SetFilterData(filter);
            fixture = fixture->GetNext();
        }
    }
    void PhysicsBody2D::EnableAllCollisions() const {
        b2Fixture* fixture = body->GetFixtureList();
        while (fixture) {
            b2Filter filter = fixture->GetFilterData();
            filter.maskBits = 0xFFFF;  // Set appropriate mask bits
            fixture->SetFilterData(filter);
            fixture = fixture->GetNext();
        }
    }

    void PhysicsBody2D::AddForce(const Vector2& force) const {
        if(!body){
            DEBUG_ERROR("Trying to apply velocity to invalid Physics Body!")
            return;
        }
        // figure out how ApplyForceToCenter works. Workaround setVelocity += force
        body->ApplyLinearImpulseToCenter((b2Vec2) force, true);
    }

    // sync between body, box2D and transforms.
    void PhysicsBody2D::SyncPhysics() {
        // cancel sync if PhysicsBody is invalid (flagged for removal, not affected by physics or invalid pointer)
        if(deleted || !isKinematic || !body) return;

        auto transformPos = transform->GetLocalPosition();
        auto bodyPos = Vector3(body->GetPosition());

        // if transform and body positions differ...
        if (transformPos != bodyPos) {
            // ... and transform was changed 'manually', set body position to transform position
            if(transform->m_positionChanged)
                body->SetTransform(transformPos, body->GetAngle());
            // ... otherwise assume physics changes and set transform position to body position.
            else
                transform->SyncPosition(Vector3(body->GetPosition()));
        }

        auto transformAngle = transform->GetGlobalRotation().z;

        // if transform.z and body rotations differ...
        if(transformAngle != body->GetAngle()){
            // ... and transform was changed 'manually', set body angle to transform z rotation.
            if(transform->m_rotationChanged)
                body->SetTransform(body->GetPosition(), transformAngle);
            // ... otherwise assume physics changes and update transform's rotation.
            else
                transform->SyncRotation(Vector3(
                        transform->GetLocalRotation().x,
                        transform->GetLocalRotation().y,
                        body->GetAngle()
                        ));
        }

        // reset transform changed values
        transform->m_positionChanged = false;
        transform->m_rotationChanged = false;

        // scale changes handled through the collider object.
    }
}