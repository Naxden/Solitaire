cmake_minimum_required(VERSION 3.15)
project(Solitaire VERSION 1.0.0)

include(FetchContent)

set(SOLITAIRE_VERSION "${PROJECT_VERSION}")
set(SOLITAIRE_BUNDLE_IDENTIFIER "com.naxden.solitaire")

if(APPLE)
  configure_file(
    ${CMAKE_SOURCE_DIR}/res/Info.plist.in
    ${CMAKE_BINARY_DIR}/Solitaire-Info.plist
    @ONLY)
endif()

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

if (APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

if(WIN32)
  set(APP_ICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")
  configure_file(${APP_ICON} ${CMAKE_BINARY_DIR}/icon.ico COPYONLY)
  set(APP_RC "${CMAKE_SOURCE_DIR}/res/solitaire.rc")
  configure_file(${APP_RC} ${CMAKE_BINARY_DIR}/solitaire.rc COPYONLY)
  target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/solitaire.rc)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${raylib_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} raylib)

if(APPLE)
  set_source_files_properties(${CMAKE_SOURCE_DIR}/assets/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/assets/icon.icns)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "${SOLITAIRE_BUNDLE_IDENTIFIER}"
    MACOSX_BUNDLE_BUNDLE_NAME "Solitaire"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Solitaire-Info.plist"
    MACOSX_BUNDLE_ICON_FILE "icon.icns"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${SOLITAIRE_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${SOLITAIRE_VERSION}")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

# C++20
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

if(APPLE)
  set(CPACK_DMG_APPLICATIONS_SYMLINK ON)
  set(CPACK_MONOLITHIC_INSTALL OFF)
  install(TARGETS Solitaire
          BUNDLE DESTINATION .
          COMPONENT SolitaireApp)
  set(CPACK_COMPONENTS_ALL SolitaireApp)
else()
  install(TARGETS Solitaire
          RUNTIME DESTINATION .
          BUNDLE  DESTINATION .)
  install(DIRECTORY assets DESTINATION .)
endif()

include(InstallRequiredSystemLibraries)

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
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Twoje imię <email>")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libasound2, libx11-6, ...")
endif()

include(CPack)