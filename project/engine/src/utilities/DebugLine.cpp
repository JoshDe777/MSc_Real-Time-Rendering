#include "engine/Game.h"
#include "engine/utilities/rendering/Material.h"
#include "engine/utilities/DebugLine.h"
#include "engine/components/Renderer.h"
#include "engine/components/meshes/Line.h"

namespace EisEngine {
    using Line = EisEngine::components::Line;
    using Renderer = EisEngine::components::Renderer;

    DebugLine::DebugLine(Game &engine, const Vector3 &startPoint, const Vector3 &endPoint, const Color& color) {
        this->engine = &engine;
        entity = &engine.entityManager.createEntity("DebugLine");
        transform = entity->transform;
        transform->SetLocalScale(Vector3::one);
        auto lineMat = Material(Vector3(color.r, color.g, color.b));
        entity->AddComponent<Renderer>(nullptr, &lineMat);
        //entity->GetComponent<LineRenderer>()->SetColor(color);
        entity->AddComponent<Line>(startPoint, endPoint);
    }

    void DebugLine::Invalidate() { engine->entityManager.deleteEntity(*entity);}

    void DebugLine::UpdateLinePosition(const Vector3 &startPoint, const Vector3 &endPoint)
    { entity->GetComponent<Line>()->SetPoints(startPoint, endPoint);}

    void DebugLine::UpdateColor(const EisEngine::Color &color)
    { entity->GetComponent<Renderer>()->material->SetDiffuse(color);}
}
