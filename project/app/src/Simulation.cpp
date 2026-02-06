#include "Simulation.h"

int n_teapots = 3;

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
        RenderingSystem::SetActiveShader("Cook-Torrance");

        skybox = std::make_shared<Skybox>(*this);

        onUpdate.addListener([&](Game& game){
            DisplayUI();
            UpdateWorld();
        });
    }

    void Simulation::DisplayUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::SetNextWindowSize(ImVec2(1000, 1000));
        // ImGui UI Window
        ImGui::Begin("Rendering Params", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Separator();
        ImGui::SeparatorText("Shininess Parameters");
        ImGui::SliderFloat("Fresnel Factor (F0)", &spec, 0.0f, 3.0f);
        ImGui::SliderFloat("Absorption Factor", &amb, 0.0f, max_absorption);
        ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::ColorEdit3("Refraction indices (eta)", &emission.x);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Simulation::UpdateWorld() {
        // blinn-phong: roughness should be inverted
        // cook-torrance: specular highlights too strong; add some attenuation
        for(const auto& teapot: pots){
            if (roughness == 0.0f)
                roughness = 0.001f;
            teapot->setRoughness(roughness);
            teapot->setOpacity(opacity);
        }
        /*for(const auto& light: lights){
            light->SetColor(emission);
            light->SetIntensity(intensity);
            light->SetRotationSpeed(spd);
        }*/
        RenderingSystem::SetSpecularFactor(spec);
        RenderingSystem::SetAmbientLevel(amb);
        RenderingSystem::SetEta(emission);
    }
}
