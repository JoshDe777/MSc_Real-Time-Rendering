#include "Simulation.h"

int n_teapots = 3;
std::vector<std::string> shaders = {"Blinn-Phong", "Cook-Torrance"};
int shaderIndex = 1;

namespace RTR {
    inline Vector3 estimatePosition(const int& index){
        if(index == 0)
            return Vector3(0, -1, 0);
        else if (index == 1)
            return Vector3(-7, 1, -1);
        else
            return Vector3(7, 1, 1);
    }

    Simulation::Simulation(const std::string& renderer) : Game("RTR Simulation") {
        // init lights & teapots here
        camera.transform->SetGlobalPosition(worldOffset);
        for(int i = 0; i < n_teapots; i++){
            pots.push_back(make_shared<Teapot>(*this));
            auto pos = estimatePosition(i);
            pots[i]->entity->transform->SetGlobalPosition(pos);
            RenderingSystem::MarkAsLoader(pots[i]->entity.get());
            lights.push_back(make_shared<Light>(*this, pots[i]->entity->transform, 2.0f));
            lights[i]->lightTransform->SetParent(pots[i]->entity->transform);
        }
        RenderingSystem::SetSpecularFactor(spec);
        RenderingSystem::SetActiveShader(shaders[shaderIndex]);
        // is it worth bringing back in UI Sliders?? I'm sure we have some from the GLIII project!?

        onAfterUpdate.addListener([&](Game& game){
            DisplayUI();
            UpdateWorld();
        });
    }

    void Simulation::DisplayUI() {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui UI Window
        ImGui::Begin("Lighting Controls");
        ImGui::Text("Adjust lighting parameters:");
        ImGui::Separator();
        ImGui::SliderFloat("Specular Strength", &spec, 0.0f, 2.0f);
        ImGui::SliderFloat("Shininess", &shiny, 1.0f, 128.0f);
        ImGui::Separator();
        ImGui::ColorEdit3("Light Color", &emission.x);
        // Intensity slider?
        ImGui::End();
    }

    void Simulation::UpdateWorld() {
        for(const auto& teapot: pots){
            // set roughness to 1-shiny
        }
        for(const auto& light: lights){
            // set emission color
        }
        RenderingSystem::SetSpecularFactor(spec);
        RenderingSystem::SetActiveShader(shaders[shaderIndex]);
    }
}
