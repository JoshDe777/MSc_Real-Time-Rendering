#pragma once

#include "Airplane.h"

#include <EisEngine.h>

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation();
    private:
        shared_ptr<Airplane> airplane = nullptr;
        Vector3 worldOffset = Vector3(0, 0, 10);
        Vector3 posVals = Vector3::zero;
        Vector3 rotationVals = Vector3::zero;
        float scale = 1.0f;
        bool readonly = false;

        void DisplayUI();
    };
}
