cmake_minimum_required(VERSION 3.30)
project(geditor C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DBUILDSTYLE_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "-g0 -O3 -s -DNDEBUG -DBUILDSTYLE_RELEASE")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O3 -DBUILDSTYLE_DEBUG")

set(CMAKE_C_FLAGS "-Wno-deprecated-declarations")

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

find_package(json-c CONFIG)
include_directories(${json-c_INCLUDE_DIRS})

add_executable(
        geditor
        main.c
        defines.h
        Helpers/List.c
        Helpers/List.h
        UI/MainWindow.c
        UI/MainWindow.h
        Helpers/Vector2.c
        Helpers/Vector2.h
        Helpers/Drawing.c
        Helpers/Drawing.h
        Helpers/Input.c
        Helpers/Input.h
        Editor.c
        Editor.h
        UI/OptionsWindow.c
        UI/OptionsWindow.h
        Helpers/Options.c
        Helpers/Options.h
        Helpers/LevelWriter.c
        Helpers/LevelWriter.h
        Helpers/GameInterface.c
        Helpers/GameInterface.h
        UI/IOWindow.c
        UI/IOWindow.h
        UI/KvWindow.c
        UI/KvWindow.h
        Helpers/KVList.c
        Helpers/KVList.h
        UI/UiHelpers.c
        UI/UiHelpers.h
)

target_link_libraries(geditor ${GTK4_LIBRARIES} m z json-c)

# Copy the runtime assets to the build directory ("Assets" folder)
add_custom_target(
        copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/Assets
)
add_dependencies(geditor copy_assets)

