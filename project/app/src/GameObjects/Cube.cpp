#include "Cube.h"

namespace Maze {
    Cube::Cube(Game& game) {
        entity = &game.entityManager->createEntity("Cube");
        entity->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        auto tex = ResourceManager::GenerateTextureFromFile(
                "textures/wood_inlaid_stone_wall_diff_4k.jpg",
                "wall_inlaid_stone_wall"
                );
        entity->AddComponent<Renderer>(tex, nullptr);
        entity->transform->SetLocalPosition(Vector3::zero);
    }
}
