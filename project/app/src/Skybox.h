#pragma once

#include <EisEngine.h>

namespace RTR {
    class Skybox {
    public:
        explicit Skybox(Game& engine);
        Entity* getEntity() {return entity.get();}
    private:
        shared_ptr<Entity> entity;
        const std::vector<std::string> skyboxPaths = {
                "textures/skybox/angkor_wat.jpg",
                "textures/skybox/bangkok_palace.jpg",
                "textures/skybox/hands_bridge.jpg",
                "textures/skybox/pistes_contamines.jpg",
                "textures/skybox/singapore_jewel.jpg",
                "textures/skybox/thanh_hue.jpg"
        };
    };
}
