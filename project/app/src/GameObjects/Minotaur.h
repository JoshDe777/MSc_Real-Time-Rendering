#pragma once

#include <EisEngine.h>

namespace Maze {
    class Minotaur {
    public:
        explicit Minotaur(Game& game);
        shared_ptr<Entity> entity;
    private:
        Game& game;
    };
}
