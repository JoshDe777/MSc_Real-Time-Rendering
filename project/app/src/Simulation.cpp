#include "Simulation.h"

int n_teapots = 1;

namespace RTR {
    inline Vector3 estimatePosition(const int& index){
        if(index == 0)
            return Vector3(0, -1, 0);
        else if (index == 1)
            return Vector3(-7, 1, -1);
        else
            return Vector3(7, 1, 1);
    }

    Simulation::Simulation() : Game("RTR Simulation") {
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
        RenderingSystem::SetActiveShader("Blinn-Phong");

        //skybox = std::make_shared<Skybox>(*this);

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
        ImGui::SliderFloat("Specular Factor", &spec, 0.0f, 3.0f);
        ImGui::SliderFloat("Ambient factor", &amb, 0.0f, max_absorption);
        ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::ColorEdit3("Light Emission Color", &emission.x);
        ImGui::Separator();
        ImGui::SeparatorText("Cube Transform Data");
        ImGui::SliderFloat3("Cube Position", &cubePos.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("Cube Rotation", &cubeRot.x, -360.0f, 360.0f);
        if(ImGui::Button("Reset Cube Transform")){
            cubePos = Vector3::zero;
            cubeRot = Vector3::zero;
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Simulation::UpdateWorld() {
        for(const auto& teapot: pots){
            teapot->entity->transform->SetLocalPosition(cubePos);
            teapot->entity->transform->SetLocalRotation(cubeRot);
            if (roughness == 0.0f)
                roughness = 0.001f;
            teapot->setRoughness(roughness);
            //teapot->setOpacity(opacity);
        }
        for(const auto& light: lights)
            light->SetColor(emission);

        RenderingSystem::SetSpecularFactor(spec);
        RenderingSystem::SetAmbientLevel(amb);
    }
}
