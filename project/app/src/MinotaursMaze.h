#pragma once

#include <EisEngine.h>

#include "GameObjects/Steve.h"
#include "GameObjects/Minotaur.h"
#include "GameObjects/Torch.h"
#include "Scripts/CamController.h"
#include "MapGeneration/ProceduralMaze.h"

using namespace Maze;
using namespace Maze::Map;

class MinotaursMaze : public Game {
public:
    explicit MinotaursMaze();
private:
    void DisplayUI();

    shared_ptr<Steve> steve = nullptr;
    shared_ptr<CamController> controller = nullptr;
    shared_ptr<ProceduralMaze> maze = nullptr;
    shared_ptr<Minotaur> minotaur = nullptr;
};
