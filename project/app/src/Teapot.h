#pragma once

#include <EisEngine.h>

namespace RTR {
    class Teapot {
    public:
        explicit Teapot(Game &game);
        void setRoughness(const float& val);
        void setOpacity(const float& val);
        void updateTextures();
        shared_ptr<Entity> entity;
    private:
        shared_ptr<Renderer> renderer;
        const std::vector<std::pair<std::string, std::string>> texPaths = {
                {"textures/red_brick_diff_2k.jpg", "textures/red_brick_nor_gl_2k.png"},
                {"textures/metal_plate_02_diff_2k.jpg", "textures/metal_plate_02_nor_gl_2k.png"},
                {"textures/asphalt_02_diff_2k.jpg", "textures/asphalt_02_nor_gl_2k.png"}
        };
        std::vector<std::pair<shared_ptr<Texture2D>, shared_ptr<Texture2D>>> textures = {};
        unsigned int texIndex = 0;
    };
}
