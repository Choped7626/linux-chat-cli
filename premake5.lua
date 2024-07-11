workspace "chat"
  architecture "x64"
  startproject "server"

  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }

  outputdir = "%{cfg.buildcfg}"

  CppVersion = "C++latest"
  MsvcToolset = "v143"
  WindowsSdkVersion = "10.0"
  
  function DeclareMSVCOptions()
    filter "system:windows"
    staticruntime "Off"
    systemversion (WindowsSdkVersion)
    toolset (MsvcToolset)
    cppdialect (CppVersion)

    defines
    {
      "_CRT_SECURE_NO_WARNINGS",
      "NOMINMAX",
      "WIN32_LEAN_AND_MEAN"
    }
    
    disablewarnings
    {
        "4100", -- C4100: unreferenced formal parameter
        "4201", -- C4201: nameless struct/union
        "4307", -- C4307: integral constant overflow
        "4311", -- C4311: 'variable' : pointer truncation from 'type' to 'type'
        "4302", -- C4302: 'conversion' : truncation from 'type 1' to 'type 2'
        "4267", -- C4267: 'var' : conversion from 'size_t' to 'type', possible loss of data
        "4244"  -- C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data
    }
  end

  function DeclareDebugOptions()
    filter "configurations:Debug"
      defines { "_DEBUG" }
      symbols "On"
	  flags { "MultiProcessorCompile" }
    filter "not configurations:Debug"
      defines { "NDEBUG" }
  end

project "client"
  kind "ConsoleApp"
  language "C++"
  targetdir "bin/%{cfg.buildcfg}"
  objdir "bin/int/%{cfg.buildcfg}/%{prj.name}"

  files
  {
    "chat/src/**.cpp",
    "chat/include/**.h",
    "shared/src/**.cpp",
    "shared/include/**.h"
  }
  
  includedirs
  {
    "chat/include",
    "shared/include"
  }

  DeclareMSVCOptions()
  DeclareDebugOptions()

  flags { "NoImportLib", "Maps", "MultiProcessorCompile" }

  filter "configurations:Debug"
    defines { "chat_DEBUG" }

  filter "configurations:Release"
    defines { "chat_RELEASE" }
    optimize "speed"

  filter "configurations:Dist"
    flags { "LinkTimeOptimization", "FatalCompileWarnings" }
    defines { "chat_DIST" }
    optimize "speed"

project "server"
  kind "ConsoleApp"
  language "C++"
  targetdir "bin/%{cfg.buildcfg}"
  objdir "bin/int/%{cfg.buildcfg}/%{prj.name}"

  files
  {
    "server/src/**.cpp",
    "server/include/**.h",
    "shared/src/**.cpp",
    "shared/include/**.h"
  }

  includedirs
  {
    "server/include",
    "shared/include"
  }

  DeclareMSVCOptions()
  DeclareDebugOptions()

  flags { "NoImportLib", "Maps", "MultiProcessorCompile" }

  filter "configurations:Debug"
    defines { "chat_DEBUG" }

  filter "configurations:Release"
    defines { "chat_RELEASE" }
    optimize "speed"

  filter "configurations:Dist"
    flags { "LinkTimeOptimization", "FatalCompileWarnings" }
    defines { "chat_DIST" }
    optimize "speed"