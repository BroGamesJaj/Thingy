cmake_minimum_required(VERSION 3.20)

project(Thingy LANGUAGES CXX)

# Define configurations, including custom 'Dist'
set(CMAKE_CONFIGURATION_TYPES Debug Release Dist CACHE STRING "Available build configurations" FORCE)
set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:CONSOLE")

set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(OBJ_DIR "${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/*.cpp"
)

add_executable(Thingy ${PROJECT_FILES})

set_target_properties(Thingy PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${OBJ_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${OBJ_DIR}
)

target_include_directories(Thingy PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL/include"
)

if (MSVC)
    target_precompile_headers(Thingy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/tpch.h")
endif()

if (WIN32)
    target_compile_definitions(Thingy PRIVATE "T_PLATFORM_WINDOWS")
    target_compile_options(Thingy PRIVATE "/utf-8")
endif()

add_subdirectory(Thingy/vendor/spdlog)
target_link_libraries(Thingy PRIVATE spdlog::spdlog)

target_include_directories(Thingy PRIVATE "Thingy/vendor/SDL/include")
find_library(SDL3_LIB SDL3
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL/sdl_build/Debug/"
    NO_DEFAULT_PATH
)
target_compile_definitions(Thingy PRIVATE SDL_MAIN_HANDLED)
if (SDL3_LIB)
    target_link_libraries(Thingy PRIVATE ${SDL3_LIB})
else()
    message(FATAL_ERROR "SDL3 library not found!")
endif()

add_custom_command(TARGET Thingy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL/sdl_build/Debug/SDL3.dll"
    $<TARGET_FILE_DIR:Thingy>
)

# Handle configurations
target_compile_definitions(Thingy PRIVATE
    $<$<CONFIG:Debug>:SE_DEBUG>
    $<$<CONFIG:Release>:SE_RELEASE>
    $<$<CONFIG:Dist>:SE_DIST>
)

target_compile_options(Thingy PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)
