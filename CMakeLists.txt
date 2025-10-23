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

install(TARGETS Solitaire
        RUNTIME DESTINATION .            # Windows/Linux binarny
        BUNDLE  DESTINATION .)           # macOS .app
install(DIRECTORY assets DESTINATION .)

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
  set(CPACK_DMG_CODESIGN_IDENTITY "Developer ID Application: YourCompany (TEAMID)")
else()
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Twoje imię <email>")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libasound2, libx11-6, ...")
endif()

include(CPack)