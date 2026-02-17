#include "Simulation.h"

int n_teapots = 1;

namespace RTR {
    inline Vector3 estimatePosition(const int& index){
        if(index == 0)
            return Vector3(0, 0, 0);
        else if (index == 1)
            return Vector3(-7, 0, 0);
        else
            return Vector3(7, 0, 0);
    }

    Simulation::Simulation() : Game("RTR Simulation") {
        // init lights & teapots here
        camera.transform->SetGlobalPosition(worldOffset);

        teapot = make_shared<Teapot>(*this, 0);
        auto pos = estimatePosition(0);
        teapot->entity->transform->SetGlobalPosition(pos);

        onUpdate.addListener([&](Game& game){
            DisplayUI();
            UpdateWorld();
        });
    }

    void Simulation::DisplayUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui UI Window
        ImGui::Begin("Sprite Transforms", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SliderFloat3("Uniform Position Offset", &uniformPos.x, -maxDist, maxDist / 10.0f);
        ImGui::SliderFloat3("Uniform Rotation Offset", &uniformRot.x, -maxRotation, maxRotation);
        ImGui::SliderFloat("Uniform Scale", &uniformScale, 0.001f, maxScale);
        if(ImGui::Button("Reset Transforms")){
            uniformPos = Vector3::zero;
            uniformRot = Vector3::zero;
            uniformScale = 1.0f;
        }
        if(ImGui::Button("Showcase Preset")){
            uniformPos = Vector3(0, -1.5f, -12);
            uniformRot = Vector3(-0.24f, 0, 0);
            uniformScale = maxScale;
        }

        ImGui::Separator();
        ImGui::SeparatorText("Material Properties");
        ImGui::SliderFloat("Tiling factor", &tiling, 0.001f, 10.0f);
        if(ImGui::Button("Reset Tiling")) {
            tiling = 1.0f;
        }
        if(ImGui::Button("NEAREST filtering")){
            teapot->setFilterMode(0);
        }
        if(ImGui::Button("LINEAR filtering")){
            teapot->setFilterMode(1);
        }
        if(ImGui::Button("MipMap Nearest filtering")){
            teapot->setFilterMode(2);
        }
        if(ImGui::Button("MipMap Linear filtering")){
            teapot->setFilterMode(3);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    void Simulation::UpdateWorld() {
        teapot->entity->transform->SetLocalPosition(uniformPos);
        teapot->entity->transform->SetLocalRotation(uniformRot * 360);
        teapot->entity->transform->SetLocalScale(Vector3(uniformScale, uniformScale, uniformScale)
        );

        teapot->entity->GetComponent<Renderer>()->material->SetTiling(tiling);
    }
}
