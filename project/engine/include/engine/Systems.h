#pragma once

#include "engine/systems/SceneGraphPruner.h"
#include "engine/systems/SceneGraphUpdater.h"

#include "engine/systems/RenderingSystem.h"

#include "engine/systems/PhysicsSystem.h"
#include "engine/systems/PhysicsUpdater.h"

#include "engine/systems/Camera.h"
#include "engine/systems/Input.h"
#include "engine/systems/Time.h"
#include "ResourceManager.h"

using SceneGraphPruner = EisEngine::systems::SceneGraphPruner;
using SceneGraphUpdater = EisEngine::systems::SceneGraphUpdater;

using RenderingSystem = EisEngine::systems::RenderingSystem;

using PhysicsSystem = EisEngine::systems::PhysicsSystem;
using PhysicsUpdater = EisEngine::systems::PhysicsUpdater;

using Camera = EisEngine::systems::Camera;
using Input = EisEngine::Input;
using Time = EisEngine::systems::Time;
