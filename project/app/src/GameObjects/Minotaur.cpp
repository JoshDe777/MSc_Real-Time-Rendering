#include "Minotaur.h"

namespace Maze {
    Minotaur::Minotaur(EisEngine::Game &game) : game(game) {
        entity = ResourceManager::Load3DObject(game, "3d-objects/minotaur.fbx");
        entity->transform->SetLocalScale(Vector3(0.025f, 0.025f, 0.025f));
        entity->transform->SetLocalPosition(Vector3::zero);
    }
}
