#include "CamController.h"

namespace Maze {
    using CameraMode = EisEngine::systems::CameraMode;

    CamController::CamController(Game& game, Entity* minotaur, Entity* steve){
        camera = game.camera.get();
        this->minotaur = minotaur->transform;
        this->steve = steve->transform;
        game.onUpdate.addListener([&](Game& game){
            Update(game);
        });

        startFwd = camera->transform->Forward();
        startUp = camera->transform->Up();
    }

    void CamController::Update(EisEngine::Game &game) {
        auto moveModifier = Time::deltaTime * movementSpeed;
        if(Input::GetKeyDown(KeyCode::W)) {
            auto moveVector = isPerspective ? camera->transform->Forward() : camera->transform->Up();
            moveVector.y = 0;
            camera->transform->Translate(moveVector * moveModifier);
        }
        if(Input::GetKeyDown(KeyCode::A)) {
            auto moveVector = -camera->transform->Right();
            moveVector.y = 0;
            camera->transform->Translate(moveVector * moveModifier);
        }
        if(Input::GetKeyDown(KeyCode::S)) {
            auto moveVector = isPerspective ? -camera->transform->Forward() : -camera->transform->Up();
            moveVector.y = 0;
            camera->transform->Translate(moveVector * moveModifier);
        }
        if(Input::GetKeyDown(KeyCode::D)) {
            auto moveVector = camera->transform->Right();
            moveVector.y = 0;
            camera->transform->Translate(moveVector * moveModifier);
        }
        if(Input::GetKeyDown(KeyCode::Q)) {
            // rotate left
            baseRotation.y += rotationSpeed * Time::deltaTime;
        }
        if(Input::GetKeyDown(KeyCode::E)) {
            // rotate right
            baseRotation.y -= rotationSpeed * Time::deltaTime;
        }
        if(Input::GetKeyDown(KeyCode::Space)) {
            if(isPerspective){
                auto moveVector = Vector3::up;
                camera->transform->Translate(moveVector * flyModifier * moveModifier);
            }
            else
                camera->Zoom(-zoomSpeed);
        }
        if(Input::GetKeyDown(KeyCode::LeftControl)) {
            if(isPerspective){
                auto moveVector = -Vector3::up;
                camera->transform->Translate(moveVector * flyModifier * moveModifier);
            }
            else
                camera->Zoom(zoomSpeed);
        }

        if(Input::GetKeyDown(KeyCode::F)){
            if(camera->transform->parent() != minotaur){
                camera->transform->SetParent(minotaur);
                camera->transform->SetLocalPosition(thirdPersonOffset + Vector3::up*3);
                camera->transform->SetLocalRotation(Vector3(-45, -15, 0));
                camera->SetCameraMode(CameraMode::PERSPECTIVE);
                isPerspective = true;
                onFocusHold.invoke(*this);
            }
            else{
                auto pos = camera->transform->GetGlobalPosition();
                camera->transform->SetParent(nullptr);
                camera->transform->SetLocalPosition(pos);
                camera->SetCameraMode(CameraMode::PERSPECTIVE);
                isPerspective = true;
            }
        }
        else if(Input::GetKeyDown(KeyCode::G)){
            if(camera->transform->parent() != steve){
                camera->transform->SetParent(steve);
                camera->transform->SetLocalPosition(thirdPersonOffset);
                camera->transform->SetLocalRotation(Vector3(-45, -15, 0));
                camera->SetCameraMode(CameraMode::PERSPECTIVE);
                isPerspective = true;
                onFocusHold.invoke(*this);
            }
            else{
                auto pos = camera->transform->GetGlobalPosition();
                camera->transform->SetParent(nullptr);
                camera->transform->SetLocalPosition(pos);
                camera->SetCameraMode(CameraMode::PERSPECTIVE);
                isPerspective = true;
            }
        }
        else if (Input::GetKeyDown(KeyCode::V)){
            if (isPerspective) {
                GoToTopDown();
            }
            else {
                camera->transform->SetGlobalPosition(Vector3(0, 1, 0));
                camera->transform->SetLocalRotation(Vector3(0, 0, 0));
                camera->SetCameraMode(CameraMode::PERSPECTIVE);
                camera->SetZoom(0);
                isPerspective = true;
            }

        }

        if(isPerspective){
            auto mousePos = Input::MousePos();
            // offset from centre
            auto size = game.context->GetWindowSize();
            auto normalizedMousePos = Vector2(mousePos.x / size.x, mousePos.y / size.y);
            auto centre = Vector2(0.5f, 0.5f);
            auto centreOffset = normalizedMousePos - centre;

            auto targetX = -centreOffset.x * 90.0f;
            auto targetY = -centreOffset.y * 90.0f;

            // yRotation -> rotation around y axis -> yaw; xRotation -> rotation around x axis -> pitch.
            // Confusing, I know. Sorry.
            yRotation = yRotation + (targetX - yRotation) * sensitivity.x * Time::deltaTime;
            xRotation = Math::Clamp(xRotation + (targetY - xRotation) * sensitivity.y * Time::deltaTime, -135, 135);

            camera->transform->SetLocalRotation(baseRotation + Vector3(xRotation, yRotation, 0));
        }
    }

    void CamController::GoToTopDown() {
        camera->transform->SetGlobalPosition(Vector3(0, 99, 0));
        camera->transform->SetLocalRotation(Vector3(-90, 0, 0));
        camera->SetCameraMode(CameraMode::ORTHO);
        camera->SetZoom(-100);
        isPerspective = false;
    }
}
