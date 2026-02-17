#pragma once

#include <EisEngine.h>

namespace RTR {
    class Teapot {
    public:
        explicit Teapot(Game &game, const int& index);
        void setRoughness(const float& val);
        void setOpacity(const float& val);
        void setFilterMode(const int& val);
        shared_ptr<Entity> entity;
    private:
        shared_ptr<Renderer> renderer;
        const std::string texPath = "textures/tiles_0059_color_2k.jpg";
        shared_ptr<Texture2D> texture = nullptr;
    };
}
