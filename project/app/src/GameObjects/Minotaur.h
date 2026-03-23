#pragma once

#include <EisEngine.h>

namespace Maze {
    class Minotaur {
    public:
        explicit Minotaur(Game& game);
        Entity* entity;
    private:
        Game& game;
    };
}
