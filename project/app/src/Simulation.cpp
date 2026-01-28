#include "Simulation.h"

int n_teapots = 3;
std::vector<std::string> shaders = {"Blinn-Phong", "Cook-Torrance", "Toon"};
int shaderIndex = 0;

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
        ImGui::SliderFloat("Specular Strength", &spec, 0.0f, 10.0f);
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::SeparatorText("Light Parameters");
        ImGui::ColorEdit3("Light Color", &emission.x);
        ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 20.0f);
        ImGui::SliderFloat("Light Speed Modifier", &spd, 1.0f, 100.0f);
        if(shaderIndex == 2)
            ImGui::SliderInt("Toon Lighting Steps", &n_levels, 1, 12);
        ImGui::Separator();
        ImGui::SeparatorText("Shader Selection");
        if(ImGui::Button("Blinn-Phong"))
            shaderIndex = 0;
        if(ImGui::Button("Cook-Torrance"))
            shaderIndex = 1;
        if(ImGui::Button("Toon"))
            shaderIndex = 2;
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
        }
        for(const auto& light: lights){
            light->SetColor(emission);
            light->SetIntensity(intensity);
            light->SetRotationSpeed(spd);
        }
        RenderingSystem::SetSpecularFactor(spec);
        RenderingSystem::SetActiveShader(shaders[shaderIndex]);
        if(shaderIndex == 2)
            RenderingSystem::SetToonLevelCount(n_levels);
    }
}
