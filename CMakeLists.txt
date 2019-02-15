cmake_minimum_required(VERSION 3.9)

project(game)

set(
    SRCS
        src/main.cpp
        src/PlayerComponent.cpp
)

add_subdirectory(blink-game-engine/)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${BLINK_LIBRARIES})

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
)

include_directories(${BLINK_INCLUDE_DIRS})

add_custom_target(
        copy_assets
        ${CMAKE_COMMAND}
        -D SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -D DESTINATION_DIR=${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/CopyAssets.cmake
        COMMENT "Copying Files for target: ${PROJECT_NAME}" VERBATIM
)

add_dependencies(${PROJECT_NAME} copy_assets)