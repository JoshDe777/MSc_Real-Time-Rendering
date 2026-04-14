# MSc_Real-Time-Rendering
Contains the working code and deliverables for my Real-Time Rendering module in my Master's course at Trinity College

### Build access instructions:
- Download the file `builds/RTR_25353241.zip`
- Extract all files to wherever you want to store the build
- Run `Game.exe`, and you should be able to see the demo!

All source code here relies on a game engine developed over university projects both during my undergraduate degree, and the first semester of my Master's course at TCD. It is mostly made by hand, building on external libraries such as Assimp, or box2D (not in use for this project, but linked nonetheless) (all referenced in the `project/extern` folder).

As per project requirements, it is an engine built in C++ using OpenGL as a graphics API, accessed through GLAD/GLFW, and extended for the purposes of this project as such.
You may find the base versions of this project under the following git repositories:
- overview of all changes made for Assignment 5 specifically (commit comparison between end of Assignment 4 and most recent): https://github.com/JoshDe777/MSc_Real-Time-Rendering/compare/b7bb1aa..1544db2
- Engine state at the end of semester 1 (direct starting point for any C++/OpenGL assignment done this semester): https://github.com/JoshDe777/MSc-CompGraphics

### Discretionary notes about the scene setup.

The demo's scene showcases a procedurally generated labyrinth featuring a Minotaur model in its middle, as well as a Steve entity that walks above the maze. ALL OF THIS WAS ALREADY IMPLEMENTED FOR MY PROJECT IN THE MODULE CS7GV6 Computer Graphics! The only changes to the visible scene I have made for this project were the inclusion of the UI menu for interactivity with the light system, and reducing the size of the maze for performance reasons! I cannot any claim credit for this scene in this module! Please keep this in mind while grading this project. (This affects every file in the folder `project/app`).

In terms of reasoning, I believed this to be a very good baseline considering my main reason for choosing this topic was hoping to deal with the heavy performance issues I experienced with this project specifically, so I figured it could be a good measure for comparison. It features a lot of meshes (original sized maze featured over 5.000, this adjusted version just around 1.000, in both cases there is some variance based on the paths generated with the maze) to render every frame, meaning the performance change between the lighting systems would show well in my eyes when I decided this.
