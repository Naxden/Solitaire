cmake_minimum_required(VERSION 3.10.0)
project(Solitaire VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SRC_FILES src/*.cpp)

add_executable(Solitaire
    main.cpp
    ${SRC_FILES}
)