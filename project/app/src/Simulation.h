#pragma once

#include <EisEngine.h>

#include "Teapot.h"
#include "Light.h"

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation(const std::string& renderer);
    private:
        shared_ptr<Entity> light_source = nullptr;
        std::vector<shared_ptr<Teapot>> pots = {};
        std::vector<shared_ptr<Light>> lights = {};
        Vector3 worldOffset = Vector3(0, -2, 10);
        float spec = 4.0f;
        float roughness = 1.0f;
        Vector3 emission = Vector3::one;
        float intensity = 1.0f;
        int n_levels = 8;
        float spd = 0.1f;

        void DisplayUI();
        void UpdateWorld();
    };
}
