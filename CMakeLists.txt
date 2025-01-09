cmake_minimum_required(VERSION 3.20)

project(Thingy LANGUAGES CXX)

# Define configurations, including custom 'Dist'
set(CMAKE_CONFIGURATION_TYPES Debug Release Dist CACHE STRING "Available build configurations" FORCE)
set(CMAKE_CXX_FLAGS_DIST "")
set(CMAKE_EXE_LINKER_FLAGS_DIST "")

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

add_subdirectory(Thingy/vendor/spdlog)
target_link_libraries(Thingy PRIVATE spdlog::spdlog)