#pragma once

#include "Airplane.h"

#include <EisEngine.h>

namespace RTR {
    class Simulation : public Game {
    public:
        explicit Simulation();
    private:
        LogPriority prio = LogPriority::ErrorP;
        shared_ptr<Airplane> airplane = nullptr;
        Vector3 worldOffset = Vector3(0, 0, 25);
        Vector3 posVals = Vector3::zero;
        Vector3 rotationVals = Vector3::zero;
        float scale = 1.0f;
        float animSpd = 3.0f;
        bool readonly = false;
        bool inAnim = false;

        void DisplayUI();
    };
}
