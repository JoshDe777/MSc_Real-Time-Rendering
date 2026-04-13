#pragma once

#include <EisEngine.h>

namespace Maze {
    class CamController {
        using event_t = Event<CamController, CamController&>;
    public:
        explicit CamController(Game& game, Entity* minotaur, Entity* steve);
        event_t onFocusHold;
        void GoToTopDown();
    private:
        void Update(Game& game);

        Camera* camera = nullptr;
        Transform* minotaur = nullptr;
        Transform* steve = nullptr;

        bool isPerspective = true;
        bool freeRoam = true;

        float movementSpeed = 0.25;
        float flyModifier = 20;
        float rotationSpeed = 50;
        float maxRotationDeg = 360;
        float zoomSpeed = 0.1f;
        float moveSpeed = 0.25f;

        Vector3 thirdPersonOffset = Vector3(0, 1, 2);

        Vector3 startFwd;
        float xRotation = 0.0f;
        Vector3 startUp;
        float yRotation = 0.0f;
        Vector2 sensitivity = Vector2(3.0f, 1.0f);
        Vector3 baseRotation = Vector3::zero;
    };
}
