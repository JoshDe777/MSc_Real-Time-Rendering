#pragma once

#include "Tile.h"
#include "../GameObjects/Torch.h"

#include <EisEngine.h>

#include <array>

namespace Maze::Map {
    class ProceduralMaze {
    public:
        explicit ProceduralMaze(Game& game);
        Tile* FindClosestTile(const Vector2& pos) const;
        Tile* GetTileAt(const Vector2& pos) const;
        Entity* env = nullptr;
    private:
        void Generate(const Vector2& centre = Vector2::zero, const int& width = 32);
        void RandomWalk(const Vector2& start,
                        std::vector<std::vector<unsigned int>>& grid,
                        std::vector<std::unique_ptr<Tile>>& map,
                        const Vector2& globalCentre
                        );
        void PlaceTile(const Tile& tile);
        Game& game;
        Entity* path = nullptr;
        Entity* walls = nullptr;
        std::vector<std::unique_ptr<Tile>> map = {};
        std::vector<std::unique_ptr<Torch>> torches = {};

        float wallSize = 3.0f;
    };
}
