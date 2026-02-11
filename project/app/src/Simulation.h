#pragma once

#include <EisEngine.h>

#include "Teapot.h"
#include "Light.h"

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation();
    private:
        shared_ptr<Entity> light_source = nullptr;
        std::vector<shared_ptr<Teapot>> pots = {};
        std::vector<shared_ptr<Light>> lights = {};
        Vector3 worldOffset = Vector3(0, -1, 2);
        float spec = 1.0f;
        float amb = 0.3f;
        float roughness = 1.0f;
        Vector3 emission = Vector3::one;
        Vector3 cubePos = Vector3::zero;
        Vector3 cubeRot = Vector3::zero;
        float intensity = 1.0f;
        float spd = 1.0f;
        float opacity = 0.1f;
        float max_eta = 1.0f;
        float max_absorption = 10.0f;

        void DisplayUI();
        void UpdateWorld();
    };
}
