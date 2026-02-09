#pragma once

#include <EisEngine.h>

#include "Teapot.h"
#include "Light.h"
#include "Skybox.h"

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation(const std::string& renderer);
    private:
        shared_ptr<Entity> light_source = nullptr;
        std::vector<shared_ptr<Teapot>> pots = {};
        std::vector<shared_ptr<Light>> lights = {};
        //shared_ptr<Skybox> skybox = nullptr;
        Vector3 worldOffset = Vector3(0, -2, 10);
        float spec = 1.0f;
        float amb = 0.3f;
        float roughness = 1.0f;
        Vector3 emission = Vector3::one;
        float intensity = 1.0f;
        float spd = 1.0f;
        float opacity = 0.1f;
        float max_eta = 1.0f;
        float max_absorption = 10.0f;

        void DisplayUI();
        void UpdateWorld();
    };
}
