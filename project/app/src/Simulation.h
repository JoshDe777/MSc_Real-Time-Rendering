#pragma once

#include <EisEngine.h>

#include "Teapot.h"
#include "Light.h"

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation();
    private:
        shared_ptr<Teapot> teapot = nullptr;
        Vector3 worldOffset = Vector3(0, 0, 5);
        Vector3 uniformPos = Vector3::zero;
        Vector3 uniformRot = Vector3::zero;
        float uniformScale = 1.0f;
        float tiling = 1.0f;
        bool uniform = false;

        const float maxDist = 40;
        const float maxRotation = 1.0f;
        const float maxScale = 30.0f;

        void DisplayUI();
        void UpdateWorld();
    };
}
