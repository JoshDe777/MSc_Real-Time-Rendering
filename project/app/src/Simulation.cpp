#include "Simulation.h"

int n_teapots = 3;

namespace RTR {
    Simulation::Simulation() : Game("RTA Assignment 1") {
        Debug::SetPriority(prio);
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

        if(readonly || inAnim){
            auto transform = airplane->getPlane()->transform;
            posVals = transform->GetLocalPosition();
            rotationVals = transform->GetLocalRotation();
            ImGui::BeginDisabled();
        }
        ImGui::SliderFloat3("World Position", &posVals.x, -100, 10);
        ImGui::SliderFloat3("Euler Rotation", &rotationVals.x, 0.0f, 360);
        ImGui::SliderFloat("Scale", &scale, 0.001f, 2.0f);

        ImGui::Separator();
        ImGui::SeparatorText("Settings");
        if(ImGui::Button("Reset Transform")){
            posVals = Vector3::zero;
            rotationVals = Vector3::zero;
            scale = 1.0f;
        }
        if(ImGui::Button("Gimbal Lock Display")){
            rotationVals = Vector3(90, rotationVals.y, rotationVals.z);
        }

        if(readonly || inAnim)
            ImGui::EndDisabled();
        else{
            auto transform = airplane->getPlane()->transform;
            transform->SetLocalPosition(posVals);
            transform->SetLocalEulerRotation(rotationVals);
            transform->SetLocalScale(Vector3(scale, scale, scale));
        }

        if(ImGui::Button(readonly ? "Unlock UI Editing" : "Lock UI Editing")){
            readonly = !readonly;
        }

        ImGui::Separator();
        ImGui::SeparatorText("Animation Settings");

        if(ImGui::Button(airplane->IsLooping() ? "End Anim at last keyframe" : "Enable animation looping"))
            airplane->ToggleLooping();
        if(ImGui::Button(airplane->IsPaused() ? "Resume animation" : "Pause animation")){
            airplane->TogglePlay();
            inAnim = !airplane->IsPaused();
        }
        if(ImGui::Button("Reset anim"))
            airplane->ResetAnim();

        ImGui::Separator();
        ImGui::SeparatorText("Interpolation modes");
        if(ImGui::Button("Raw linear"))
            airplane->SetAnimMode(AnimMode::RAW);
        if(ImGui::Button("Uniform linear"))
            airplane->SetAnimMode(AnimMode::UNIFORM);

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
