# [ROOT] PIXEL-PALADIN

cmake_minimum_required(VERSION 3.25)
project(PixelPaladin LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(engine)