cmake_minimum_required(VERSION 3.15)

project(Solitaire VERSION 1.0.1 LANGUAGES CXX)

# ------------------------------------------------------------------
# Project metadata
# ------------------------------------------------------------------
include(GNUInstallDirs)
include(FetchContent)

set(SOLITAIRE_VERSION "${PROJECT_VERSION}")
set(SOLITAIRE_BUNDLE_IDENTIFIER "com.naxden.solitaire")

if(APPLE)
  configure_file(
    ${CMAKE_SOURCE_DIR}/res/Info.plist.in
    ${CMAKE_BINARY_DIR}/Solitaire-Info.plist
    @ONLY)
endif()

# ------------------------------------------------------------------
# Third-party dependencies
# ------------------------------------------------------------------
set(SOLITAIRE_RAYLIB_VERSION 5.5 CACHE STRING "Raylib version to fetch")

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_RAYLIB_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(INSTALL_RAYLIB OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/${SOLITAIRE_RAYLIB_VERSION}.tar.gz
  FIND_PACKAGE_ARGS ${SOLITAIRE_RAYLIB_VERSION}
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# Guard against third-party install() rules leaking into our package payload.
set(_solitaire_prev_skip_install_defined FALSE)
if(DEFINED CMAKE_SKIP_INSTALL_RULES)
  set(_solitaire_prev_skip_install "${CMAKE_SKIP_INSTALL_RULES}")
  set(_solitaire_prev_skip_install_defined TRUE)
endif()
set(CMAKE_SKIP_INSTALL_RULES ON)

FetchContent_MakeAvailable(raylib)

FetchContent_GetProperties(raylib)
if(raylib_BINARY_DIR AND NOT EXISTS "${raylib_BINARY_DIR}/cmake_install.cmake")
  file(WRITE "${raylib_BINARY_DIR}/cmake_install.cmake" "# raylib install disabled\n")
endif()

if(_solitaire_prev_skip_install_defined)
  set(CMAKE_SKIP_INSTALL_RULES "${_solitaire_prev_skip_install}")
else()
  unset(CMAKE_SKIP_INSTALL_RULES)
endif()
unset(_solitaire_prev_skip_install)
unset(_solitaire_prev_skip_install_defined)

# ------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SOURCE_FILES})

if(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${raylib_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    raylib
    $<$<PLATFORM_ID:Darwin>:-framework;Cocoa;-framework;IOKit;-framework;OpenGL>
)

if(WIN32)
  target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/res/solitaire.rc)
endif()

if(APPLE)
  set_source_files_properties(${CMAKE_SOURCE_DIR}/assets/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/assets/icon.icns)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "${SOLITAIRE_BUNDLE_IDENTIFIER}"
    MACOSX_BUNDLE_BUNDLE_NAME "Solitaire"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Solitaire-Info.plist"
    MACOSX_BUNDLE_ICON_FILE "icon.icns"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${SOLITAIRE_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${SOLITAIRE_VERSION}"
  )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# ------------------------------------------------------------------
# Packaging & installation
# ------------------------------------------------------------------
set(CPACK_MONOLITHIC_INSTALL OFF)

if(APPLE)
  set(CPACK_DMG_APPLICATIONS_SYMLINK ON)
  install(TARGETS Solitaire
          BUNDLE DESTINATION .
          COMPONENT SolitaireApp)
  set(CPACK_COMPONENTS_ALL SolitaireApp)
elseif(WIN32)
  install(TARGETS Solitaire RUNTIME DESTINATION .)
  install(DIRECTORY assets DESTINATION .)
else()
  set(SOLITAIRE_APP_DESTINATION "${CMAKE_INSTALL_BINDIR}/Solitaire" CACHE STRING "Install path for the game assets")
  install(TARGETS Solitaire RUNTIME DESTINATION ${SOLITAIRE_APP_DESTINATION})
  install(DIRECTORY assets DESTINATION ${SOLITAIRE_APP_DESTINATION})
  set(APP_SHRTCT "${CMAKE_SOURCE_DIR}/res/solitaire.desktop")
  configure_file(${APP_SHRTCT} ${CMAKE_BINARY_DIR}/solitaire.desktop COPYONLY)
  install(FILES ${CMAKE_BINARY_DIR}/solitaire.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
  install(FILES ${CMAKE_SOURCE_DIR}/assets/icon.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps
    RENAME solitaire.png)
endif()

set(CPACK_PACKAGE_NAME "Solitaire")
set(CPACK_PACKAGE_VENDOR "Naxden")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/Naxden/Solitaire")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(WIN32)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
  set(CPACK_PACKAGE_EXECUTABLES "Solitaire" "Solitaire")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "Solitaire.exe")
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")
  set(CPACK_NSIS_ENABLE_SIGNING ON)
  set(CPACK_NSIS_SIGNING_COMMAND "signtool sign /fd sha256 /a /f \"${SIGN_PFX}\" /p \"${SIGN_PWD}\" \"${CPACK_TEMPORARY_PACKAGE_LOCATION}\"")
  set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_PACKAGE_FILE_NAME "Solitaire-${PROJECT_VERSION}")
else()
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Naxden")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Solitaire game made with C++ and Raylib")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/")
endif()

include(CPack)