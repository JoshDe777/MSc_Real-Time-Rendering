#include "engine/utilities/AnimState.h"

namespace EisEngine {
    bool AnimState::run(const float &elapsedTime) {
        onRun(elapsedTime);
        auto exit = exitCondition();
        if(exit)
            onExiting();

        return exit;
    }
}
