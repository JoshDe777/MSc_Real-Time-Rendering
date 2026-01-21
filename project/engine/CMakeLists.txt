cmake_minimum_required(VERSION 3.18)

set(ENGINE_NAME Engine)

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "include/**.h")
file(GLOB_RECURSE PRIVATE_HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")

add_library(${ENGINE_NAME} ${HEADER_LIST} ${PRIVATE_HEADER_LIST} ${SOURCE_LIST})

target_include_directories(${ENGINE_NAME} PUBLIC include/)
target_link_libraries(${ENGINE_NAME} PUBLIC glad glfw glm soloud tinygltf box2d stb fastnoise assimp::assimp)

target_compile_definitions(${ENGINE_NAME} PRIVATE ASSET_ROOT=./assets/)

# copy assets to output directory before build
add_custom_command(TARGET ${ENGINE_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${ENGINE_NAME}>/assets)
target_compile_features(${ENGINE_NAME} PRIVATE cxx_std_17)
set_target_properties(${ENGINE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
