#pragma once

#include <EisEngine.h>

namespace RTR {
    class Teapot {
    public:
        explicit Teapot(Game &game);
        void setRoughness(const float& val);
        void setOpacity(const float& val);
        shared_ptr<Entity> entity;
    private:
        shared_ptr<Renderer> renderer;
        void rotate();
        float rotationSpeed = 3.0f;
        const std::string diffuseTexPath = "textures/metal_plate_02_diff_2k.png";
        const std::string normalTexPath = "textures/metal_plate_02_nor_gl_2k.png";
    };
}
