cmake_minimum_required(VERSION 3.25)
project(3d_renderer C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_DIR}/include)

add_executable(${PROJECT_NAME} main.c display.h display.c vector.h vector.c vector.c mesh.h mesh.c triangle.h triangle.c array.h array.c)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main)
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
)