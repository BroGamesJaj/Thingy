workspace "Thingy"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

Outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
--IncludeDir["SDL"] = "SpaceEngine/vendor/SDL/include"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
--IncludeDir["imGui"] = "Thingy/vendor/imgui"

IncludeLib = {}
--IncludeLib["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

--include "Thingy/vendor/SDL"
--include "Thingy/vendor/imGui"

project "Thingy"
    location "Thingy"
    kind "ConsoleApp"
    language "C++"

    pchheader "tpch.h"
    pchsource "Thingy/src/tpch.cpp"

    targetdir ("bin/" .. Outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. Outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        --"Thingy/vendor/spdlog/include",
        "src/",
        --"%{IncludeDir.SDL}",
        --"%{IncludeDir.VulkanSDK}",
        --"%{IncludeDir.imGui}"
    }

    libdirs {
        --"%{IncludeLib.VulkanSDK}"
    }

    links
        {
            --"vulkan-1.lib",
            --"SDL",
            --"imgui"
        }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "T_PLATFORM_WINDOWS"
        }

        buildoptions
        {
            "/utf-8"
        }

    filter "configurations:Debug"
        defines "SE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "SE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "SE_DIST"
        optimize "On"