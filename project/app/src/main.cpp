#include <iostream>

#include "Simulation.h"

using Simulation = RTR::Simulation;

int main(){
    std::cout << "Hello World!" << std::endl;
    Debug::SetPriority(DebugP);
    auto sim = Simulation();
    sim.run();
}