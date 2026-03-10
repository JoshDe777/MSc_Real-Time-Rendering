cmake_minimum_required(VERSION 3.21)
project(rtr-project)

set(CMAKE_CXX_STANDARD 17)

# Prevents Box2D errors on first CMake compilation :D
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/box2d)
add_subdirectory(extern/tinygltf)
add_subdirectory(extern/soloud)
add_subdirectory(extern/stb)
add_subdirectory(extern/fastnoise)
add_subdirectory(extern/assimp)

# Triggers an error because some external dependencies changed stuff around otherwise.
if(MSVC)
    target_compile_options(assimp PRIVATE /wd4392)
endif()

add_subdirectory(engine)
add_subdirectory(app)
