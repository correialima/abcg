cmake_minimum_required(VERSION 3.11)

project(doodlejump)

add_executable(${PROJECT_NAME} main.cpp window.cpp player.cpp platforms.cpp clouds.cpp)

enable_abcg(${PROJECT_NAME})
