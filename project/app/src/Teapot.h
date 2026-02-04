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
    };
}
