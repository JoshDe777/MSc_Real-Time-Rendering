#include "ProceduralMaze.h"

namespace Maze::Map {

#pragma region helpers
    bool SufficientlyExplored(std::vector<std::vector<unsigned int>>& grid, const int& width, const float& percent){
        float sum = 0;
        for(const auto& dim : grid)
            for(const auto& val : dim)
                sum += (float) val;

        // since val = {0, 1} => width * width = max/full exploration.
        return sum >= percent * (float) width * (float) width;
    }

    std::vector<Vector2> GetUnexploredTiles(std::vector<std::vector<unsigned int>>& grid){
        std::vector<Vector2> res = {};
        int len = grid.size();

        for(auto i = 0; i < len; i++)
            for(auto j = 0; j < len; j++)
                if (grid[i][j] == 0)
                    res.emplace_back(i, j);

        return res;
    }

    const std::array<Vector2, 4> cardinalDirections
        {Vector2(0, 1), Vector2(1, 0), Vector2(0, -1), Vector2(-1, 0)};

    int to_arrIndex (const Vector2& dir) {
        if(dir == Vector2::up - Vector2::right)
            return 0;
        if(dir == Vector2::up)
            return 1;
        if(dir == Vector2::up + Vector2::right)
            return 2;
        if(dir == -Vector2::right)
            return 3;
        if(dir == Vector2::zero)
            return 4;
        if(dir == Vector2::right)
            return 5;
        if(dir == -Vector2::up - Vector2::right)
            return 6;
        if(dir == -Vector2::up)
            return 7;
        if(dir == -Vector2::up + Vector2::right)
            return 8;

        return -1;
    }

    Vector2 IndexToVec2(const unsigned int& i){
        switch(i){
            case 0:
                return Vector2::up - Vector2::right;
            case 1:
                return Vector2::up;
            case 2:
                return Vector2::up + Vector2::right;
            case 3:
                return -Vector2::right;
            case 4:
                return Vector2::zero;
            case 5:
                return Vector2::right;
            case 6:
                return -Vector2::up - Vector2::right;
            case 7:
                return -Vector2::up;
            default:
                return -Vector2::up + Vector2::right;
        }
    }

    Vector2 arrayToGlobalPos(const Vector2& pos, const Vector2& offset){
        return offset + 3*pos;
    }

    Vector2 globalToArrayPos(const Vector2& pos, const Vector2& offset){
        return (pos - offset) / 3;
    }

    bool InMap(std::vector<std::unique_ptr<Tile>>& map, const Vector2& tile){
        return std::any_of(map.begin(), map.end(), [&](std::unique_ptr<Tile>& t) {return t->pos == tile;});
    }

    bool IsValid(const Vector2& pos, std::vector<std::vector<unsigned int>>& grid, std::vector<Vector2>& path){
        // return true if pos in grid && not in path
        auto inGrid = pos.x >= 0 && pos.y >= 0 && pos.x < grid.size() - 1 && pos.y < grid[0].size() - 1;
        auto notInPath = !std::any_of(path.begin(), path.end(),
                                    [&](Vector2& tile){return tile == pos;});
        return inGrid && notInPath;
    }

    // return true only if pattern of 1-1-1 in EITHER x or y direction.
    bool IsCorridor(const std::array<unsigned int, 9>& grid){
        return (grid[3] == 1 && grid[5] == 1) == (grid[1] == 0 && grid[7] == 0) !=
                (grid[1] == 1 && grid[7] == 1) == (grid[3] == 0 || grid[5] == 0);
    }

    Vector3 GetRight(const Vector3& dir){
        return dir == Vector3::forward ? Vector3::right :
            dir == Vector3::right ? -Vector3::forward :
            dir == -Vector3::forward ? -Vector3::right :
            dir == -Vector3::right ? Vector3::forward : Vector3::zero;
    }

    float GetTorchRotation(const int& i){
        switch(i){
            case 1:
                return 180;
            case 3:
                return -90;
            case 5:
                return 90;
            default:
                return 0;
        }
    }
#pragma endregion

    int torchCounter = 0;
    int tileSkip = 5;

    void ProceduralMaze::PlaceTile(const Tile& tile){
        // 0 = wall; 1 = path/walkable.
        std::array<unsigned int, 9> localWalls{
                0, 0, 0,
                0, 1, 0,
                0, 0, 0
        };
        for(auto dir : cardinalDirections){
            int neighbourExists = tile.IsNeighbour(tile.pos + 3*dir);
            try{
                localWalls[to_arrIndex(dir)] = neighbourExists;
            }
            catch(std::exception& e){
                DEBUG_ERROR("Invalid array index: " + std::string(e.what()));
            }
        }

        // build meshes here:
        std::string name = "Path " + (std::string) tile.pos;
        auto tilePath = game.entityManager->createEntity(name);
        tilePath->transform->SetParent(path->transform);
        // make vec3 with z = pos.y
        tilePath->transform->SetLocalPosition(Vector3(tile.pos.x, 0, tile.pos.y));

        std::string wallName = "Wall " + (std::string) tile.pos;
        auto tileWall = game.entityManager->createEntity(wallName);
        tileWall->transform->SetParent(walls->transform);
        tileWall->transform->SetLocalScale(Vector3(1, wallSize, 1));
        tileWall->transform->SetLocalPosition(Vector3(tile.pos.x, wallSize/2, tile.pos.y));

        std::vector<Vector3> pathVertices = {};
        std::vector<unsigned int> pathIndices = {};
        std::vector<Vector2> pathUVs = {};

        std::vector<Vector3> wallVertices = {};
        std::vector<unsigned int> wallIndices = {};
        std::vector<Vector3> wallNormals = {};
        std::vector<Vector2> wallUVs = {};

        for(auto i = 0; i < localWalls.size(); i++){
            auto offset = IndexToVec2(i);
            if(localWalls[i] > 0){
                int nVertices = (int) pathVertices.size();
                for(auto v : PrimitiveMesh2D::Square.GetVertices()){
                    auto v2D = v + offset;
                    pathVertices.emplace_back(v2D.x, 0, v2D.y);
                }

                for(auto& v: PrimitiveMesh2D::Square.indices)
                    pathIndices.emplace_back(v + nVertices);

                pathUVs.emplace_back(0, 1);
                pathUVs.emplace_back(0, 0);
                pathUVs.emplace_back(1, 0);
                pathUVs.emplace_back(1, 1);
            }
            else{
                int nVertices = (int) wallVertices.size();
                for(auto v: PrimitiveMesh3D::cube.GetVertices())
                    wallVertices.emplace_back(v.x + offset.x, v.y, v.z + offset.y);

                for(auto& ind : PrimitiveMesh3D::cube.indices)
                    wallIndices.emplace_back(ind + nVertices);

                for(auto& n : PrimitiveMesh3D::cube.GetNormals())
                    wallNormals.push_back(n);

                for (auto& uv : PrimitiveMesh3D::cube.GetUVs())
                    wallUVs.push_back(uv);
            }
        }

        std::vector<Vector3> pathNormals((int)pathVertices.size(), Vector3::up);

        tilePath->AddComponent<Mesh3D>(PrimitiveMesh3D (
                pathVertices, pathIndices, &pathNormals, &pathUVs)
        );

        // texturing here

        auto pathTexture = ResourceManager::GetTexture("path");
        if(!pathTexture)
            pathTexture = ResourceManager::GenerateTextureFromFile("textures/gravelly_sand_diff_4k.jpg", "path");
        auto pathRenderer = tilePath->AddComponent<Renderer>(pathTexture);
        pathRenderer->material->SetTiling(4.0f);

        tileWall->AddComponent<Mesh3D>(PrimitiveMesh3D(
                wallVertices, wallIndices, &wallNormals, &wallUVs)
        );

        auto wallTexture = ResourceManager::GetTexture("walls");
        if(!wallTexture)
            wallTexture = ResourceManager::GenerateTextureFromFile("textures/wood_inlaid_stone_wall_diff_4k.jpg", "walls");
        auto wallRenderer = tileWall->AddComponent<Renderer>(wallTexture);
        wallRenderer->material->SetTiling(4.0f);

        if(Math::Mod((float) torchCounter++, (float) tileSkip) != 0)
            return;

        float torchY = 0.2f;
        // place torches:
        // for each val in cardinal dirs where val = 1
        // place on right hand side going out
        if (IsCorridor(localWalls)) {
            auto dir = localWalls[3] == 1 ? -Vector3::right : Vector3::forward;
            float rotation = dir == -Vector3::right ? -90 : 180;
            // place torch @ pos + 0.5 * GetRight(dir) + torchY;
            auto torch = torches.emplace_back(make_unique<Torch>(game)).get();
            Vector3 pos = Vector3(tile.pos.x, torchY, tile.pos.y) + GetRight(dir) * 0.5f;
            torch->entity->transform->SetGlobalPosition(pos);
            torch->entity->transform->Rotate(Vector3(0, rotation, 0));
        }
        else
            // foreach pos where grid = 1:
            for(auto i = 0; i < 9; i++){
                if (localWalls[i] == 0)
                    continue;
                // place torch @ pos + 0.5 * GetRight(pos - centre) + torchY;
                auto dirVec2 = IndexToVec2(i);
                // skip centre tile
                if(dirVec2 == Vector2::zero)
                    continue;
                auto dir = Vector3(dirVec2.x, 0, dirVec2.y);
                auto torch = torches.emplace_back(make_unique<Torch>(game)).get();
                Vector3 pos = dir + Vector3(tile.pos.x, torchY, tile.pos.y) + GetRight(dir) * 0.5f;
                torch->entity->transform->SetGlobalPosition(pos);
                torch->entity->transform->Rotate(Vector3(0, GetTorchRotation(i), 0));
            }
    }

    ProceduralMaze::ProceduralMaze(Game &game) : game(game) {
        env = game.entityManager->createEntity("Maze");

        path = game.entityManager->createEntity("Maze Path");
        path->transform->SetParent(env->transform);

        walls = game.entityManager->createEntity("Maze Walls");
        walls->transform->SetParent(env->transform);

        Generate(Vector2::zero, mazeWidth);
    }

    void ProceduralMaze::Generate(const Vector2& centre, const int& width) {
        // init: generate grid as std::array<std::array<unsigned int, width>, width> with all values = 0;
        // 0 = unvisited, 1 = visited.
        std::vector<std::vector<unsigned int>> grid(width, std::vector<unsigned int>(width, 0));
        // compute all four exit coords (all permutations of 0, +- width/2 => even number = same index each time.)
        auto half = (int) (width / 2);
        std::array exit_ids{
            Vector2(0.0f, (float) half),
            Vector2((float) half, 0),
            Vector2((float)width-1, (float) half),
            Vector2((float) half, (float) width-1)};

        // gen loop
        // do tile linking @ end of walks to keep path progression instead of grid-based path estimation
        // coords = global centre + grid index - grid centre
        // do random walk from start
        RandomWalk(Vector2((float) half, (float) half), grid, map, centre);
        // random walks from all exits
        for(const auto& v: exit_ids)
            RandomWalk(v, grid, map, centre);
        // random walks from random, unvisited cells until >= 95% visits (sum array >= 0.95f * width**2)
        while(!SufficientlyExplored(grid, width, 0.95f)){
            // get unvisited cells & select random
            auto unvisitedCells = GetUnexploredTiles(grid);
            auto cellIndex = RandomInt(0, (int) unvisitedCells.size() - 1);
            RandomWalk(unvisitedCells[cellIndex], grid, map, centre);
        }

        // population
        // create walls & floors from tilemap x grid => unexplored = walls
        // delete tiles with no neighbours.
        map.erase(
                std::remove_if(map.begin(), map.end(),[](const std::unique_ptr<Tile>& tile){
                    return tile->neighbours.empty();
                }),
                map.end()
            );

        for(auto& tile : map){
            PlaceTile(*tile);
            // add textures (biome mapping here if implemented)
        }
    }

    void ProceduralMaze::RandomWalk(const Vector2 &start, std::vector<std::vector<unsigned int>> &grid,
                                    std::vector<std::unique_ptr<Tile>> &map, const Vector2& globalCentre) {
        std::vector<Vector2> currentPath = {};
        grid [(int) start.x][(int) start.y] = 1;
        Vector2 offset = globalCentre - 3 * Vector2((float) (int) grid.size() / 2, (float) (int) grid.size() / 2);

        currentPath.push_back(start);

        Vector2 current = start;

        // end random walk if joining the maze.
        while(true){
            if(InMap(map, current))
                break;

            // establish a net of valid neighbours (cardinal direction && position not in path && in grid)
            std::vector<Vector2> neighbours = {};
            for(auto dir : cardinalDirections){
                auto n = current + dir;
                if (IsValid(n, grid, currentPath))
                    neighbours.push_back(n);
            }
            // exit if no neighbours
            if(neighbours.empty())
                break;

            // choose random neighbour
            auto nextPos = neighbours[RandomInt(0, (int)neighbours.size() - 1)];
            // create Tile object & link to current
            currentPath.push_back(nextPos);
            // set current = neighbour
            current = nextPos;
        }

        // add all tiles in path to map & connect
        Tile* last = nullptr;
        for(auto i = 0; i < (int) currentPath.size(); i++){
            // get tile position in global space
            grid[(int) currentPath[i].x][(int) currentPath[i].y] = 1;
            auto globalPos = arrayToGlobalPos(currentPath[i], offset);
            // search for existing tile (nullptr if none)
            auto tile = GetTileAt(globalPos);
            // create new one if none
            if(!tile){
                map.push_back(std::make_unique<Tile>(globalPos));
                tile = map.back().get();
            }

            // if first tile in path, continue to next (dead end)
            if(last){
                // connect this tile with the previous tile for the pathing.
                tile->AddNeighbour(last);
                last->AddNeighbour(tile);
            }

            last = tile;
        }
    }

    Tile *ProceduralMaze::GetTileAt(const Vector2 &pos) const {
        for(auto& t : map)
            if(pos == t->pos)
                return t.get();
        return nullptr;
    }

    Tile *ProceduralMaze::FindClosestTile(const Vector2 &pos) const {
        if(map.empty())
            return nullptr;

        const Tile* closest = nullptr;
        float closestDist = 100000000.0f;

        std::for_each(map.begin(), map.end(), [&](auto& t){
            auto tile = *t.get();
            if(!closest){
                closest = &tile;
                closestDist = Vector2::Distance(tile.pos, pos);
                return;
            }

            if(Vector2::Distance(tile.pos, pos) < closestDist){
                closest = &tile;
                closestDist = Vector2::Distance(tile.pos, pos);
            }
        });

        return const_cast<Tile*>(closest);
    }
}
