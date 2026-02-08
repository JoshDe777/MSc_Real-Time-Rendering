#include "Airplane.h"

namespace RTR {
    Airplane::Airplane(EisEngine::Game &game) {
        auto entity = ResourceManager::Load3DObject(game, assetPath);
        plane = static_cast<shared_ptr<Entity>>(entity);
        plane->transform->PrintRelativeSceneGraph();
        for(auto child: plane->transform->getChildren()){
            child->SetLocalPosition(childOffsets);
            child->SetLocalRotation(rotationOffsets);
            child->SetLocalScale(startScale);

            // this scale doesn't seem to get applied but to the parent it does...?
            // also material is a) pitch black, and b) the plane's front doesn't display at all...?
        }
    }
}
