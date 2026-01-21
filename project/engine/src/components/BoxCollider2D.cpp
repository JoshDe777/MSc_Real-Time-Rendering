#include "engine/components/BoxCollider2D.h"
#include "engine/components/PhysicsBody2D.h"
#include "engine/components/meshes/Mesh2D.h"
#include "engine/components/meshes/SpriteMesh.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
    using namespace ecs;
    using EisEngine::rendering::PrimitiveMesh;
    using EisEngine::components::PhysicsBody2D;

// helper functions:
    // utility struct to determine whether a class derives from PrimitiveMesh.
    template<typename T>
    struct is_primitive_mesh : std::is_base_of<PrimitiveMesh, T> { };

    // Builds a box around a mesh definition. Doesn't execute if T doesn't derive from PrimitiveMesh
    template<typename P>
    Bounds2D GetBoxBoundaries(const P &primitive, const Vector3 &scale,
                              typename std::enable_if<is_primitive_mesh<P>::value, P>::type * = nullptr) {
        auto results = Bounds2D(
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()
        );

        for (auto vertex: primitive.GetVertices()) {
            float scaledX = vertex.x * scale.x;
            float scaledY = vertex.y * scale.y;
            if (scaledX < results.left)
                results.left = scaledX;
            if (scaledX > results.right)
                results.right = scaledX;
            if (scaledY < results.bottom)
                results.bottom = scaledY;
            if (scaledY > results.top)
                results.top = scaledY;
        }

        results.ReevaluateSize();

        return results;
    }

// BoxCollider2D functions:

    BoxCollider2D::BoxCollider2D(Game &engine, guid_t owner) :
            Component(engine, owner),
            bounds(SetDefaultBounds()) {}

    void BoxCollider2D::EnterCollision(BoxCollider2D &other) {
        OnCollisionEnter.invoke(other);
    }

    void BoxCollider2D::ExitCollision(BoxCollider2D &other) {
        OnCollisionExit.invoke(other);
    }

    void BoxCollider2D::Invalidate() {
        OnCollisionEnter.reset();
        OnCollisionExit.reset();
        Component::Invalidate();
    }

    // bounds management:
    void BoxCollider2D::SetBounds(EisEngine::Bounds2D newBounds) { bounds = newBounds;}

    Bounds2D BoxCollider2D::SetDefaultBounds(const Bounds2D &_defaultBounds) {
        auto *spriteMesh = entity()->GetComponent<SpriteMesh>();
        auto *mesh2D = entity()->GetComponent<Mesh2D>();
        // if no mesh found, create collider with bounds sized 1x1 (defaultBounds object)
        if (!spriteMesh && !mesh2D)
            return _defaultBounds;
        // otherwise return the boundaries drawn around the found mesh type.
        // DISCLAIMER: not very extensible, might consider making this a separate GetBoundaries(Mesh* mesh) function.
        return spriteMesh ?
               GetBoxBoundaries(spriteMesh->primitive, entity()->transform->GetGlobalScale()) :
               GetBoxBoundaries(mesh2D->primitive, entity()->transform->GetGlobalScale());
    }

    void BoxCollider2D::RescaleBounds(const Vector3 &oldScale, const Vector3 &scale)
    { bounds.normalize(oldScale) *= scale;}
}
