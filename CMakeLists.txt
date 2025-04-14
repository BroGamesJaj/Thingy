cmake_minimum_required(VERSION 3.20)

project(Thingy LANGUAGES CXX)

# Define configurations.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build configurations" FORCE)
set(CMAKE_CXX_FLAGS "/EHsc -wd4129")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

# Output directories
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(OBJ_DIR "${CMAKE_BINARY_DIR}/bin-int/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")



# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SDL3_DIR "${CMAKE_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build")
set(SDL3_mixer_DIR "${CMAKE_SOURCE_DIR}/Thingy/vendor/SDL_mixer/build")

# Collect project files
file(GLOB_RECURSE PROJECT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/*.cpp"
)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/Resource.rc")

# Create executable
add_executable(Thingy ${PROJECT_FILES} ${APP_ICON_RESOURCE_WINDOWS})

# Set target properties
set_target_properties(Thingy PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${OBJ_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${OBJ_DIR}
)

# Include directories
target_include_directories(Thingy PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/libs"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL/include"
)

target_link_libraries(Thingy PRIVATE "${CMAKE_SOURCE_DIR}/Thingy/vendor/spdlog/build/Debug/spdlogd.lib")

# Precompiled header setup
if (MSVC)
    target_compile_options(Thingy PRIVATE "$<$<CONFIG:Debug>:/Zi>")
    target_link_options(Thingy PRIVATE "$<$<CONFIG:Debug>:/DEBUG>")
    target_precompile_headers(Thingy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/src/tpch.h")
endif()

# Platform-specific configurations for Windows
if (WIN32)
    target_compile_definitions(Thingy PRIVATE "T_PLATFORM_WINDOWS")
    target_compile_options(Thingy PRIVATE "/utf-8")
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build")
find_package(SDL3 REQUIRED)

# SDL3 include directories
target_include_directories(Thingy PRIVATE 
    "Thingy/vendor/SDL_mixer/external/SDL/include"
    "Thingy/vendor/SDL_mixer/include"
)

# SDL3 link libraries
target_compile_definitions(Thingy PRIVATE SDL_MAIN_HANDLED)

# Link SDL_mixer
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES MATCHES "Release")
    target_link_libraries(Thingy PRIVATE SDL3::SDL3 "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/build/Release/SDL3_mixer.lib")
else()
    target_link_libraries(Thingy PRIVATE SDL3::SDL3 "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/build/Debug/SDL3_mixer.lib")
endif()

# Link SDL3
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES MATCHES "Release")
    target_link_directories(Thingy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build/Release")
else()
    target_link_directories(Thingy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build/Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES MATCHES "Release")
    add_custom_command(TARGET Thingy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build/Release/SDL3.dll"
        $<TARGET_FILE_DIR:Thingy>
    )

    add_custom_command(TARGET Thingy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/build/Release/SDL3_mixer.dll"
        $<TARGET_FILE_DIR:Thingy>
    )
else()
    add_custom_command(TARGET Thingy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/sdl_build/Debug/SDL3.dll"
        $<TARGET_FILE_DIR:Thingy>
    )

    add_custom_command(TARGET Thingy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/build/Debug/SDL3_mixer.dll"
        $<TARGET_FILE_DIR:Thingy>
    )
endif()

# CURL library setup
add_definitions(-DHAVE_CONFIG_H)

set(CURL_LIB_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-release-dll-ipv6-sspi-schannel/lib/libcurl.lib")
set(CURL_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-debug-dll-ipv6-sspi-schannel/lib/libcurl_debug.lib")

target_link_libraries(Thingy PRIVATE "$<IF:$<CONFIG:Debug>,${CURL_LIB_DEBUG},${CURL_LIB_RELEASE}>" ws2_32 wldap32 crypt32)

set(CURL_DLL_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-debug-dll-ipv6-sspi-schannel/bin/libcurl_debug.dll")
set(CURL_DLL_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-release-dll-ipv6-sspi-schannel/bin/libcurl.dll")

set(CURL_INCLUDE_DIR_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-debug-dll-ipv6-sspi-schannel/include")
set(CURL_INCLUDE_DIR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/curl/builds/libcurl-vc17-x64-release-dll-ipv6-sspi-schannel/include")

get_filename_component(CURL_INCLUDE_DIR_DEBUG_ABS ${CURL_INCLUDE_DIR_DEBUG} ABSOLUTE)
get_filename_component(CURL_INCLUDE_DIR_RELEASE_ABS ${CURL_INCLUDE_DIR_RELEASE} ABSOLUTE)

target_include_directories(Thingy PRIVATE
    "$<IF:$<CONFIG:Debug>,${CURL_INCLUDE_DIR_DEBUG_ABS},${CURL_INCLUDE_DIR_RELEASE_ABS}>"
)
# Copy the DLL to the output directory
add_custom_command(TARGET Thingy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${CURL_DLL_DEBUG},${CURL_DLL_RELEASE}>"
    "$<TARGET_FILE_DIR:Thingy>"
)

# ImGui sources and backends
set(IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/misc/cpp/imgui_stdlib.cpp"
)

set(IMGUI_BACKENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/backends/imgui_impl_sdl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp"
)

# Add ImGui as a static library
add_library(ImGui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS})

# ImGui include directories
target_include_directories(ImGui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/imgui/misc/cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thingy/vendor/SDL_mixer/external/SDL/include"
)

# Link ImGui to the project
target_link_libraries(Thingy PRIVATE ImGui)

# Compiler definitions based on build configuration
target_compile_definitions(Thingy PRIVATE
    $<$<CONFIG:Debug>:T_DEBUG>
    $<$<CONFIG:Release>:T_RELEASE>
)

# Compiler options based on build configuration
target_compile_options(Thingy PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)

file(COPY ${CMAKE_SOURCE_DIR}/Thingy/assets DESTINATION ${OUTPUT_DIR})