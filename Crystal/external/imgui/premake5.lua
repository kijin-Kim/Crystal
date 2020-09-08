project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{prj.name}/bin/%{cfg.buildcfg}/"
    objdir "bin/%{prj.name}/objs/%{cfg.buildcfg}/"

    files 
    { 
        "imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp"
    }
