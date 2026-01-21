#include "engine/components/Script.h"
#include "engine/Game.h"

namespace EisEngine::components {
    Script::Script(EisEngine::Game &engine, EisEngine::ecs::guid_t owner) : Component(engine, owner) {
        engine.onEntityStart.addListener([&] (Game &engine){ Start();});
        updateHandle = engine.onUpdate.addListener([&] (Game &engine) { Update();});
    }

    void Script::Invalidate() {
        engine.onUpdate.removeListener(updateHandle);
        Component::Invalidate();
    }
}
