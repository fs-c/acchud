cmake_minimum_required(VERSION 3.19)
project(acchud CXX)

message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 23)

add_library(acchud SHARED src/acchud.cpp src/hooks.cpp
        include/kiero/kiero.cpp
        include/imgui/imgui.cpp
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_win32.cpp
        include/imgui/backends/imgui_impl_dx11.cpp src/overlay.cpp)

target_include_directories(acchud PRIVATE include)
target_include_directories(acchud PRIVATE include/imgui)

target_link_libraries(acchud PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x64-v141-mtd.lib)

target_compile_features(acchud PRIVATE cxx_std_20)
