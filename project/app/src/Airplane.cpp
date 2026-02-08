#include "Airplane.h"

namespace RTR {
    Airplane::Airplane(EisEngine::Game &game) {
        auto entity = ResourceManager::Load3DObject(game, assetPath);
        plane = static_cast<shared_ptr<Entity>>(entity);
        for(auto child: plane->transform->getChildren()){
            //child->SetLocalPosition(startPos);
            child->SetLocalRotation(rotationOffsets);
        }
    }
}
