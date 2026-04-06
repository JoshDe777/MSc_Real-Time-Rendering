#include "Steve.h"

namespace Maze {
    Steve::Steve(Game &game) {
        auto ovrScale = Vector3(0.25f, 0.25f, 0.25f);
        entity = game.entityManager->createEntity("Steve");

        torso = game.entityManager->createEntity("Torso");
        torso->transform->SetParent(entity->transform);
        torso->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        torso->AddComponent<Renderer>()->material
            ->SetDiffuse(Color((float)98/255, (float)147/255, (float)227/255, 1));
        torso->transform->SetGlobalScale(Vector3(1, 2, 0.75f));

        neck = game.entityManager->createEntity("NeckJoint");
        neck->transform->SetParent(torso->transform);
        neck->transform->SetLocalPosition(Vector3(0, (float)5/16, 0));

        auto head = game.entityManager->createEntity("Head");
        head->transform->SetParent(neck->transform);
        head->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        head->AddComponent<Renderer>()->material
                ->SetDiffuse(Color((float)247/255, (float)228/255, (float)232/255, 1));
        head->transform->SetGlobalScale(Vector3((float)3/4, (float)3/4, (float)3/4));
        head->transform->SetLocalPosition(Vector3(0, (float)3/8, 0));

        shoulderL = game.entityManager->createEntity("ShoulderLJoint");
        shoulderL->transform->SetParent(torso->transform);
        shoulderL->transform->SetLocalPosition(Vector3(-0.7f, 0.8f, 0));

        auto armL = game.entityManager->createEntity("LeftArm");
        armL->transform->SetParent(shoulderL->transform);
        armL->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        armL->AddComponent<Renderer>()->material
            ->SetDiffuse(Color((float)98/255, (float)147/255, (float)227/255, 1));
        armL->transform->SetGlobalScale(Vector3(0.4f,2,0.4f));
        armL->transform->SetLocalPosition(Vector3(0,-0.8f, 0));

        shoulderR = game.entityManager->createEntity("ShoulderRJoint");
        shoulderR->transform->SetParent(torso->transform);
        shoulderR->transform->SetLocalPosition(Vector3(0.7f, 0.8f, 0));

        auto armR = game.entityManager->createEntity("RightArm");
        armR->transform->SetParent(shoulderR->transform);
        armR->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        armR->AddComponent<Renderer>()->material
                ->SetDiffuse(Color((float)98/255, (float)147/255, (float)227/255, 1));
        armR->transform->SetGlobalScale(Vector3(0.4f,2,0.4f));
        armR->transform->SetLocalPosition(Vector3(0,-0.8f, 0));

        hipL = game.entityManager->createEntity("HipLJoint");
        hipL->transform->SetParent(torso->transform);
        hipL->transform->SetLocalPosition(Vector3(-0.25f, 0, 0));

        auto legL = game.entityManager->createEntity("LeftLeg");
        legL->transform->SetParent(hipL->transform);
        legL->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        legL->AddComponent<Renderer>()->material->SetDiffuse(Color::blue);
        legL->transform->SetGlobalScale(Vector3(0.4f,2,0.4f));
        legL->transform->SetLocalPosition(Vector3(0,-1, 0));

        hipR = game.entityManager->createEntity("HipRJoint");
        hipR->transform->SetParent(torso->transform);
        hipR->transform->SetLocalPosition(Vector3(0.25f, 0, 0));

        auto legR = game.entityManager->createEntity("RightLeg");
        legR->transform->SetParent(hipR->transform);
        legR->AddComponent<Mesh3D>(PrimitiveMesh3D::cube);
        legR->AddComponent<Renderer>()->material->SetDiffuse(Color::blue);
        legR->transform->SetGlobalScale(Vector3(0.4f,2,0.4f));
        legR->transform->SetLocalPosition(Vector3(0,-1, 0));

        entity->transform->SetLocalScale(ovrScale);
        entity->transform->SetGlobalPosition(Vector3(0, 5, 0));

        game.onUpdate.addListener([&](Game& game){
            Animate();
        });

        auto dynamic_light = game.entityManager->createEntity("Dynamic Light");
        dynamic_light->transform->SetParent(entity->transform);
        //dynamic_light->transform->SetLocalPosition(Vector3(0, 0, 0));
        auto lightMat = ResourceManager::CreateMaterialInstance("default");
        lightMat->SetEmission(Vector3(1, 1, 1));
        lightMat->SetIntensity(100.0f);
    }

    void Steve::Animate() {
        // rotate
        if(Input::GetKeyDown(KeyCode::R))
            torso->transform->Rotate(Vector3(0,1,0) * 5.0f);

        // walk animation:
        animTime += Time::deltaTime;

        float stepAngle = Math::Sin(animTime * animSpeed) * stride;
        auto rotation = Vector3(stepAngle, 0, 0);

        shoulderL->transform->SetLocalRotation(rotation);
        hipR->transform->SetLocalRotation(rotation);
        shoulderR->transform->SetLocalRotation(-rotation);
        hipL->transform->SetLocalRotation(-rotation);

        entity->transform->Translate(torso->transform->Forward() * moveSpeed * Time::deltaTime);
    }
}
