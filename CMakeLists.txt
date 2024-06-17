cmake_minimum_required(VERSION 3.10)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/cmake_modules")

set(SDL2_PATH "C:/Program Files/SDL2/x86_64-w64-mingw32")
set(SDL2_TTF_PATH "C:/Program Files/SDL2_ttf/x86_64-w64-mingw32")

set(SOURCES
        src/main.cpp
        src/game.cpp
        src/board.cpp
        src/piece.cpp
)

include_directories(include)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})

add_executable(Tetris ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(Tetris PROPERTIES LINK_FLAGS "-mconsole")
endif()