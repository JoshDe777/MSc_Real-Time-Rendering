#pragma once

#include <EisEngine.h>

namespace RTR {
    class Airplane {
    public:
        explicit Airplane(Game& game);
        Entity* getPlane() {return plane.get();}
        // void ToggleAxes();
    private:
        shared_ptr<Entity> plane = nullptr;
        bool displayAxes = false;
        const std::string assetPath = "3d-objects/j22.fbx";
        const Vector3 startPos = Vector3(0, 0, -15);
        const Vector3 rotationOffsets = Vector3(0,90,0);
        // std::array<std::shared_ptr<Entity>, 3> axisRings;
    };
}
