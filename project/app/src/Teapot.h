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
        const std::string diffuseTexPath = "textures/red_brick_diff_2k.jpg";
        const std::string normalTexPath = "textures/red_brick_nor_gl_2k.png";
    };
}
