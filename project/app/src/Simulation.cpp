#include "Simulation.h"

int n_teapots = 3;

namespace RTR {
    Simulation::Simulation() : Game("RTA Assignment 1") {
        airplane = std::make_shared<Airplane>(*this);

        camera.transform->SetGlobalPosition(worldOffset);

        Context::SetClearColor(Color::blue);
        RenderingSystem::SetActiveShader("Unlit");

        onUpdate.addListener([&](Game& game){
            DisplayUI();
        });
    }

    void Simulation::DisplayUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin(readonly ? "Airplane Transform [locked]" : "Airplane Transform", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if(readonly){
            auto transform = airplane->getPlane()->transform;
            posVals = transform->GetLocalPosition();
            rotationVals = transform->GetLocalRotation();
            ImGui::BeginDisabled();
        }
        ImGui::SliderFloat3("World Position", &posVals.x, -100, 10);
        ImGui::SliderFloat3("Euler Rotation", &rotationVals.x, 0.0f, 360);
        ImGui::SliderFloat("Scale", &scale, 0.001f, 2.0f);
        if(ImGui::Button("Reset Transform")){
            posVals = Vector3::zero;
            rotationVals = Vector3::zero;
            scale = 1.0f;
        }
        if(readonly)
            ImGui::EndDisabled();
        else{
            auto transform = airplane->getPlane()->transform;
            transform->SetLocalPosition(posVals);
            transform->SetLocalEulerRotation(rotationVals);
            transform->SetLocalScale(Vector3(scale, scale, scale));
        }

        ImGui::Separator();
        if(ImGui::Button(readonly ? "Unlock UI Editing" : "Lock UI Editing")){
            readonly = !readonly;
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
