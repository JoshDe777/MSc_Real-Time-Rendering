#include <iostream>

#include "Simulation.h"

using Simulation = RTR::Simulation;

std::string renderer = "blinn-phong";

int main(){
    std::cout << "Hello World!" << std::endl;
    auto sim = Simulation(renderer);
    sim.run();
}