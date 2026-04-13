#include "MinotaursMaze.h"

MinotaursMaze::MinotaursMaze() : Game("Minotaur's Maze") {
    steve = make_shared<Steve>(*this);
    minotaur = make_shared<Minotaur>(*this);

    maze = make_shared<ProceduralMaze>(*this);

    camera->transform->SetLocalPosition(Vector3(0, 2, 0));

    controller = make_shared<CamController>(*this, minotaur->entity, steve->entity);
    controller->GoToTopDown();

    RenderingSystem::MarkAsLoader(minotaur->entity);
    RenderingSystem::MarkAsLoader(steve->entity);

    onAfterUpdate.addListener([&] (Game& game) { DisplayUI();});
}

void MinotaursMaze::DisplayUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Framerate", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::BeginDisabled();

    ImGui::Text("Framerate:");
    auto framerate = (1.0f / Time::deltaTime);
    std::stringstream ss;
    std::string text;
    ss << std::setprecision(2) << framerate << " fps";
    ss>>text;
    ImGui::Text("%s", (text + " fps").c_str());

    auto meshCount = componentManager->countComponentsOfType<Mesh3D>();
    ImGui::Text("%s", (std::to_string(meshCount) + " meshes rendered.").c_str());

    ImGui::EndDisabled();

    if(ImGui::Button("Reset Steve Position")){
        steve->BringBackToStart();
    }

    ImGui::Separator();
    ImGui::SeparatorText("Lighting Parameters:");
    // toggle between voxel grid & barnes-hut
    if(ImGui::Button(voxelRendering ? "Switch to Barnes-Hut Lighting" : "Switch to Voxel-based Lighting")){
        voxelRendering = !voxelRendering;
        RenderingSystem::SetLightMode(voxelRendering);
    }

    if(!voxelRendering){
        ImGui::Text("Warning, tuning these parameters too low might significantly affect performance negatively!");
        // adjust base threshold
        ImGui::SliderFloat("Base Error Threshold", &base_TH, 0.0f, 10.0f);
        ImGui::Text("(The minimum possible error threshold -> maximum degree of light accuracy on fully rendered objects.)");
        // adjust threshold steepness
        ImGui::SliderFloat("Threshold Steepness", &steepness, 0.01f, 1.0f);
        ImGui::Text("(The factor by which the threshold rises with distance from objects.\nLow steepness might result in low FPS values.)");
        // adjust threshold stretch
        ImGui::SliderFloat("Threshold Stretch", &stretch, 0.001f, 0.5f);
        ImGui::Text("(The factor by which the threshold is normalized with distance from objects.\nHigh stretch might result in low FPS values.)");

        if(ImGui::Button("Reset Values")){
            base_TH = 0.1f;
            steepness = 0.075f;
            stretch = 0.001f;
        }

        LightSystem::SetBaseBHThreshold(base_TH);
        LightSystem::SetBHThresholdSteepness(steepness);
        LightSystem::SetBHThresholdStretch(stretch);
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
