cmake_minimum_required(VERSION 3.5)
project(LabelMusic)

include_directories(include)

link_directories(bin)

set(exe_s 
    src/main.c

    src/apis/apiMap.c
    src/apis/apiGraphic.c

    src/game/game.c
    src/game/menu.c
    src/stb.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

add_executable(main ${exe_s})
add_executable(map_editor src/map_editor/main.c src/apis/apiMap.c)
target_link_libraries(main glew32s SDL2 opengl32 SDL2_ttf discord-rpc)

add_executable(mapdata_read test/mapdata.c src/apis/apiMap.c)
add_executable(mapdata_create test/mapdatacreate.c src/apis/apiMap.c)

add_executable(get_directories test/get_dir.c src/game/menu.c)

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/shaders"
        "$<TARGET_FILE_DIR:main>/shaders"
)

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:main>/data"
)
