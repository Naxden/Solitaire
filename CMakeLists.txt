cmake_minimum_required(VERSION 3.15)
project(Solitaire VERSION 0.1.0 LANGUAGES CXX)

# Ułatwia pracę narzędzi (clangd, clang-tidy)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opcje
option(SOLITAIRE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(SOLITAIRE_ENABLE_SANITIZERS "Enable ASan/UBSan in Debug (GCC/Clang)" OFF)

# Źródła
file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB HEADER_FILES CONFIGURE_DEPENDS include/*.h)

add_executable(Solitaire)
target_sources(Solitaire PRIVATE
    main.cpp
    ${SRC_FILES}
    ${HEADER_FILES} # dla lepszej widoczności w IDE
)

# Wymagaj C++20 (m.in. std::format)
target_compile_features(Solitaire PRIVATE cxx_std_20)

# Include tylko dla tego targetu
target_include_directories(Solitaire PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Ostrzeżenia zależnie od kompilatora
if (MSVC)
    target_compile_options(Solitaire PRIVATE
        /W4
        $<$<BOOL:${SOLITAIRE_WARNINGS_AS_ERRORS}>:/WX>
    )
else()
    target_compile_options(Solitaire PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<BOOL:${SOLITAIRE_WARNINGS_AS_ERRORS}>:-Werror>
    )
endif()

# Opcjonalne sanitizery w Debug dla GCC/Clang
if (SOLITAIRE_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(Solitaire PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(Solitaire PRIVATE -fsanitize=address,undefined)
endif()