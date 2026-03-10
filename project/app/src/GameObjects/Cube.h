#pragma once

#include <EisEngine.h>

namespace Maze {
    class Cube {
    public:
        explicit Cube(Game& game);

        Entity* entity = nullptr;
    };
}
