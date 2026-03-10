#pragma once

#include <EisEngine.h>

namespace Maze::Map {
    class Tile {
    public:
        explicit Tile(const Vector2& pos) : pos(pos) { }

        void AddNeighbour(const Tile* n){
            if(!IsNeighbour(n->pos))
                neighbours.push_back(n);
        }

        bool IsNeighbour(const Vector2& nPos) const {
            return std::any_of(neighbours.begin(), neighbours.end(),
                               [&](const Tile* n){return n->pos == nPos;});
        }

        std::vector<const Tile*> neighbours = {};
        const Vector2 pos;

        bool operator==(Tile& other) {return pos == other.pos;}
    };
}
