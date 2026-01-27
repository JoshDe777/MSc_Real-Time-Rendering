#pragma once

// File for convenient inclusion of all basic engine stuffs
// **DO NOT** use within *engine code*, high potential for include circles!

#include "engine/ecs/Entity.h"
#include "engine/ecs/Component.h"
#include "engine/Components.h"
#include "engine/Utilities.h"
#include "engine/Game.h"
#include "gui/imgui.h"
#include "gui/imgui_impl_opengl3.h"
#include "gui/imgui_impl_glfw.h"

using namespace EisEngine;
using Entity = EisEngine::ecs::Entity;
