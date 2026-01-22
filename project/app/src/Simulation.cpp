#include "Simulation.h"

int n_teapots = 1;

namespace RTR {
    inline Vector3 estimatePosition(const int& index){
        float x = ceil((float) index / 2) * pow(-1, index);
        auto y = (float) pow(index, 2) * 0.5f;
        auto z = - (float) pow(index, 2);
        return Vector3(x, y, z);
    }

    Simulation::Simulation(const std::string& renderer) : Game("RTR Simulation") {
        // init lights & teapots here
        camera.transform->SetGlobalPosition(worldOffset);
        DEBUG_LOG("Camera placed at " + (std::string) camera.transform->GetGlobalPosition() + ", pointing towards " + (std::string) camera.viewDirection())
        for(int i = 0; i < n_teapots; i++){
            pots.push_back(make_shared<Teapot>(*this));
            pots[i]->entity->transform->SetGlobalPosition(estimatePosition(i));
            DEBUG_INFO("Placed teapot at position " + (std::string) pots[i]->entity->transform->GetGlobalPosition())
            lights.push_back(make_shared<Light>(*this, pots[i]->entity->transform, 0.5f));
        }
        // is it worth bringing back in UI Sliders?? I'm sure we have some from the GLIII project!?
    }
}
