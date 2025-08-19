cmake_minimum_required(VERSION 3.15)
project(Solitaire)

include(FetchContent)

# Raylib
set(RAYLIB_VERSION 5.5)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${RAYLIB_VERSION}
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(raylib)


file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${raylib_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} raylib)

# C++20
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)