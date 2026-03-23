#include "MinotaursMaze.h"

MinotaursMaze::MinotaursMaze() : Game("Minotaur's Maze") {
    steve = make_shared<Steve>(*this);
    minotaur = make_shared<Minotaur>(*this);

    maze = make_shared<ProceduralMaze>(*this);

    camera->transform->SetLocalPosition(Vector3(0, 2, 0));

    controller = make_shared<CamController>(*this, minotaur->entity, steve->entity);

    RenderingSystem::MarkAsLoader(minotaur->entity);
    RenderingSystem::MarkAsLoader(steve->entity);
}

void MinotaursMaze::DisplayUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Framerate", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::BeginDisabled();

    auto framerate = (1.0f / Time::deltaTime);
    std::stringstream ss;
    std::string text;
    ss << std::setprecision(2) << framerate << " fps";
    ss>>text;
    ImGui::Text(text.c_str());

    ImGui::EndDisabled();
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
