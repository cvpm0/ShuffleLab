cmake_minimum_required(VERSION 3.16)
project(ShuffleLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(shufflelab
    main.cpp
    src/Shuffle.cpp
    src/Stats.cpp
    src/ExperimentRunner
    src/UI.cpp
    src/Report.cpp
)

target_include_directories(shufflelab PRIVATE include)
target_compile_options(shufflelab PRIVATE -O3 -Wall -Wextra)
