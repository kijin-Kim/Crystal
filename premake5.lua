workspace "Crystal"
    architecture "x64"
    configurations { "Debug", "Release"}
    startproject "Game"
    disablewarnings { "26812", "26451"}

include "Crystal/external/imgui"

project "Crystal"
    location "Crystal"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{prj.name}/bin/%{cfg.buildcfg}/"
    objdir "bin/%{prj.name}/objs/%{cfg.buildcfg}/"

    pchheader "cspch.h"
    pchsource "Crystal/src/cspch.cpp"

    links
    {
        "imgui"
	}
    
    includedirs
    {
        "Crystal/src/",
        "Crystal/external/",
        "Crystal/external/assimp/include/"
    }

    files 
    { 
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.h",
        "%{prj.name}/external/assimp/include/assimp/**.h",
        "%{prj.name}/external/assimp/include/assimp/**.hpp",
        "%{prj.name}/external/assimp/include/assimp/**.cpp",
    }

   
    filter "configurations:Debug"
		defines "CS_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "CS_RELEASE"
		optimize "on"




project "Game"
    location "Game"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{prj.name}/bin/%{cfg.buildcfg}/"
    objdir "bin/%{prj.name}/objs/%{cfg.buildcfg}/"

    links 
    {
        "Crystal"
    }

    includedirs
    {
        "Crystal/src"
    }

    files 
    { 
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.h",
    }

    filter "configurations:Debug"
		defines "CS_DEBUG"
		symbols "on"

        links
        {
            "Crystal/external/assimp/bin/Debug/assimp-vc142-mtd"
		}

        postbuildcommands
        {
            '{COPY} "../Crystal/external/assimp/bin/Debug/assimp-vc142-mtd.dll" "%{cfg.targetdir}"'
        }

	filter "configurations:Release"
		defines "CS_RELEASE"
		optimize "on"

        links
        {
            "Crystal/external/assimp/bin/Release/assimp-vc142-mt"
		}

        postbuildcommands
        {
            '{COPY} "../Crystal/external/assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"'
        }