
newoption {
	trigger = "with-sff",
	description = "Enable generation of single file format",
}

newoption {
	trigger = "with-dx11",
	description = "Enable DX11 backend sample",
}

newoption {
	trigger = "with-opengl",
	description = "Enable OpenGL backend sample",
}

newoption {
	trigger = "with-glfw",
	description = "Enable GLFW and OpenGL backend sample",
}

newoption {
	trigger = "with-sokol",
	description = "Enable Sokol backend sample",
}

newoption {
	trigger = "with-bgfx",
	description = "Enable BGFX backend sample (need bgfx/bimg/bx repositories next to ImWindow repositorie)",
}

local PROJECT_DIR          = (path.getabsolute("..") .. "/")
local PROJECT_BUILD_DIR    = path.join(PROJECT_DIR, ".build/")
local PROJECT_PROJECTS_DIR = path.join(PROJECT_DIR, ".projects")
local PROJECT_RUNTIME_DIR  = path.join(PROJECT_DIR, "Output/")
local BGFX_ROOT_DIR        = path.join(PROJECT_DIR, "..")

-- Single file format
if _OPTIONS["with-sff"] then
	print "=================="
	print "Generate SFF files"
	print "=================="

	local EOL = "\n"

	function IncludeSFF( inputFileName, outFile )
		local inputFile = io.open(inputFileName, "r")
		if nil == inputFile then
			print("Error: Can't include file " .. inputFileName)
		else
			print("Include file " .. inputFileName)
			local isInSFFArea = false
			while true do
				local line = inputFile:read()
				if line == nil then
					break
				elseif line == "//SFF_BEGIN" then
					isInSFFArea = true
				elseif line == "//SFF_END" then 
					isInSFFArea = false
				elseif isInSFFArea == true then
					outFile:write(line..EOL);
				end
			end

			outFile:write(EOL);
		end
	end

	-- Write header
	local fileSFFHeader = io.open("../ImWindow.h","w+")

	-- Write informations
	fileSFFHeader:write("// ImWindow - Single file format"..EOL)

	fileSFFHeader:write("#ifndef __IM_WINDOW_HEADER__"..EOL)
	fileSFFHeader:write("#define __IM_WINDOW_HEADER__"..EOL)

	fileSFFHeader:write("#include \"ImwConfig.h\""..EOL..EOL)

	fileSFFHeader:write("namespace ImWindow {"..EOL)
	--Forward declare classes
	fileSFFHeader:write("	class ImwContainer;"..EOL)
	fileSFFHeader:write("	class ImwPlatformWindow;"..EOL)
	fileSFFHeader:write("	class ImwWindowManager;"..EOL)
	fileSFFHeader:write(EOL)
	IncludeSFF( "../ImWindow/JsonValue.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwWindow.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwMenu.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwStatusBar.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwToolBar.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwContainer.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwPlatformWindow.h", fileSFFHeader )
	IncludeSFF( "../ImWindow/ImwWindowManager.h", fileSFFHeader )
	fileSFFHeader:write("}\n")
	fileSFFHeader:write("#endif // __IM_WINDOW_HEADER__"..EOL)
	fileSFFHeader:close()

	-- Write source
	local fileSFFSource = io.open("../ImWindow.cpp","w+")

	fileSFFSource:write("#include \"ImWindow.h\""..EOL..EOL)
	fileSFFSource:write("#include <algorithm>"..EOL..EOL)

	fileSFFSource:write("namespace ImWindow {"..EOL)
	IncludeSFF( "../ImWindow/JsonValue.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwWindow.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwMenu.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwStatusBar.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwToolBar.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwContainer.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwPlatformWindow.cpp", fileSFFSource )
	IncludeSFF( "../ImWindow/ImwWindowManager.cpp", fileSFFSource )
	fileSFFSource:write("}"..EOL)
	fileSFFSource:close()

	print "=================="
end

function SetupSuffix()
	configuration {}
	
	configuration { "Debug", "x32" }
		targetsuffix	"_d"
	configuration { "Debug", "x64" }
		targetsuffix	"_x64_d"
	
	configuration { "Release", "x32" }
		targetsuffix	""
	configuration { "Release", "x64" }
		targetsuffix	"_x64"
	
	configuration {}
end

if _OPTIONS["with-bgfx"] then
	BGFX_DIR        = path.join(BGFX_ROOT_DIR, "bgfx")
	BX_DIR          = path.join(BGFX_ROOT_DIR, "bx")
	BIMG_DIR        = path.join(BGFX_ROOT_DIR, "bimg")

	-- Required for bgfx and example-common
	function copyLib()
	end

	function compat(_bxDir)
	-- VS
	configuration { "vs*" }
		includedirs { path.join(_bxDir, "include/compat/msvc") }

	configuration { "vs2008" }
		includedirs { path.join(_bxDir, "include/compat/msvc/pre1600") }

	-- MinGW
	configuration { "*mingw*" }
		includedirs { path.join(_bxDir, "include/compat/mingw") }

	-- OSX
	configuration { "osx* or xcode*" }
		includedirs { path.join(_bxDir, "include/compat/osx") }

	configuration {} -- reset configuration
	end
end

solution "ImWindow"
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x32", "x64" }

if _OPTIONS["with-bgfx"] then
		defines
		{
			"ENTRY_CONFIG_IMPLEMENT_DEFAULT_ALLOCATOR=1",
			"BX_CONFIG_ENABLE_MSVC_LEVEL4_WARNINGS=1",
			"BGFX_CONFIG_DEBUG=1",
		}

		configuration { "vs* or mingw-*" }
			defines
			{
				"BGFX_CONFIG_RENDERER_DIRECT3D11=1",
			}

		configuration {}

		dofile (path.join(BX_DIR, "scripts/toolchain.lua"))
		if not toolchain(PROJECT_PROJECTS_DIR, BGFX_ROOT_DIR) then
			return -- no action specified
		end
end
	
	location				(path.join(PROJECT_PROJECTS_DIR, _ACTION))
	objdir					(path.join(PROJECT_BUILD_DIR, _ACTION))

if _OPTIONS["with-bgfx"] then
		dofile (path.join(BX_DIR, "scripts/bx.lua"))
		dofile (path.join(BIMG_DIR, "scripts/bimg.lua"))
		dofile (path.join(BGFX_DIR, "scripts/bgfx.lua"))

		bgfxProject("", "StaticLib", {})
end

	-- Inlude only SFF files in project
if _OPTIONS["with-sff"] then
		project "ImWindowSFF"
			uuid			"458E707F-2347-47D2-842A-A431CA538063"
			kind			"StaticLib"
			targetdir		(PROJECT_RUNTIME_DIR)

			files {
							"../ImWindow.cpp",
							"../ImWindow.h",
							"../ImWindow/ImwConfig.h",
							"../Externals/imgui/imconfig.h",
							"../Externals/imgui/imgui.h",
							"../Externals/imgui/imgui_internal.h",
							"../Externals/imgui/imgui.cpp",
							"../Externals/imgui/imgui_draw.cpp",
							"../Externals/imgui/stb_rect_pack.h",
							"../Externals/imgui/stb_textedit.h",
							"../Externals/imgui/stb_truetype.h",
			}
			
			
			flags			{ "ExtraWarnings" }

			vpaths {
							["ImGui"] = "../Externals/imgui/**"
			}
			
			includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../Externals/imgui"
			}

			SetupSuffix()
end

	project "ImWindow"
		uuid				"99AABCFD-6ED6-43E5-BD14-EFDC04CBE09F"
		kind				"StaticLib"
		targetdir			(PROJECT_RUNTIME_DIR)

		files {
							"../ImWindow/**.cpp",
							"../ImWindow/**.h",
							"../Externals/imgui/imconfig.h",
							"../Externals/imgui/imgui.h",
							"../Externals/imgui/imgui_internal.h",
							"../Externals/imgui/imgui.cpp",
							"../Externals/imgui/imgui_draw.cpp",
							"../Externals/imgui/stb_rect_pack.h",
							"../Externals/imgui/stb_textedit.h",
							"../Externals/imgui/stb_truetype.h",
		}
		
		vpaths {
							["ImGui"] = "../Externals/imgui/**"
		}
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow/",
							"../Externals/imgui"
		}
		
		flags				"ExtraWarnings"

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()

if _OPTIONS["with-dx11"] then
	startproject "ImWindowDX11"
	project "ImWindowDX11"
		uuid				"449C0C09-919A-4337-A09A-DFC2420A41B0"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files
		{
							"../sample.h",
							"../ImWindowEasyWindow/**.cpp",
							"../ImWindowEasyWindow/**.h",
							"../ImWindowDX11/**.cpp",
							"../ImWindowDX11/**.h",
							"../Externals/EasyWindow/EasyWindow*.cpp",
							"../Externals/EasyWindow/EasyWindow*.h"
		}	
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../ImWindowEasyWindow",
							"../Externals/imgui",
							"../Externals/EasyWindow",
							"../Externals/DirectX/include"
		}
		
		configuration		"x32"
			libdirs			"../Externals/DirectX/lib/x86"
			
		configuration		"x64"
			libdirs			"../Externals/DirectX/lib/x64"

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()
end

if _OPTIONS["with-opengl"] then
	startproject "ImWindowOGL"
	project "ImWindowOGL"
		uuid				"9f3441f5-4523-4464-b05e-d9668bd99efa"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files
		{
							"../sample.h",
							"../ImWindowEasyWindow/**.cpp",
							"../ImWindowEasyWindow/**.h",
							"../ImWindowOGL/**.cpp",
							"../ImWindowOGL/**.h",
							"../Externals/EasyWindow/EasyWindow*.cpp",
							"../Externals/EasyWindow/EasyWindow*.h"
		}	
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../ImWindowEasyWindow",
							"../Externals/imgui",
							"../Externals/EasyWindow"
		}

		links {
							"OpenGL32",
							"glu32"
		}

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()
end

if _OPTIONS["with-glfw"] then
	startproject "ImWindowGLFW"
	project "ImWindowGLFW"
		uuid				"3da873e6-1acd-486c-a941-9b45018d9eca"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files
		{
							"../sample.h",
							"../ImWindowGLFW/**.cpp",
							"../ImWindowGLFW/**.h",
		}	
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../Externals/imgui"
		}

		configuration "x32"
			includedirs		"../Externals/glfw-3.2.1.bin.WIN32/include"
		configuration "x64"
			includedirs		"../Externals/glfw-3.2.1.bin.WIN64/include"

		configuration { "vs2010", "x32" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN32/lib-vc2010"
		configuration { "vs2012", "x32" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN32/lib-vc2012"
		configuration { "vs2013", "x32" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN32/lib-vc2013"
		configuration { "vs2015 or vs2017", "x32" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN32/lib-vc2015"
		
		configuration { "vs2012", "x64" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN64/lib-vc2012"
		configuration { "vs2013", "x64" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN64/lib-vc2013"
		configuration { "vs2015 or vs2017", "x64" }
			libdirs			"../Externals/glfw-3.2.1.bin.WIN64/lib-vc2015"

		configuration {}

		links {
							"glfw3",
							"OpenGL32"
		}

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()
end

if _OPTIONS["with-sokol"] then
	startproject "ImWindowSokolOGL"
	project "ImWindowSokolOGL"
		uuid				"383edfd4-2172-46db-a1b9-d05927d98249"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files
		{
							"../sample.h",
							"../ImWindowEasyWindow/**.cpp",
							"../ImWindowEasyWindow/**.h",
							"../ImWindowSokol/**.cpp",
							"../ImWindowSokol/**.h",
							"../Externals/EasyWindow/EasyWindow*.cpp",
							"../Externals/EasyWindow/EasyWindow*.h",
							"../Externals/sokol/sokol_gfx.h",
							"../Externals/flextGL/**.c",
							"../Externals/flextGL/**.h"
		}	
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../ImWindowEasyWindow",
							"../ImWindowSokol",
							"../Externals/imgui",
							"../Externals/EasyWindow",
							"../Externals/sokol",
							"../Externals/flextGL"
		}

		defines {
							"SOKOL_GLCORE33"
		}
		
		links {
							"OpenGL32",
							"glu32"
		}

		configuration		"x32"
			libdirs			"../Externals/DirectX/lib/x86"
			
		configuration		"x64"
			libdirs			"../Externals/DirectX/lib/x64"

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()

	project "ImWindowSokolDX11"
		uuid				"f3e9429c-a762-4eac-9902-ace5a4274810"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files
		{
							"../sample.h",
							"../ImWindowEasyWindow/**.cpp",
							"../ImWindowEasyWindow/**.h",
							"../ImWindowSokol/**.cpp",
							"../ImWindowSokol/**.h",
							"../Externals/EasyWindow/EasyWindow*.cpp",
							"../Externals/EasyWindow/EasyWindow*.h",
							"../Externals/sokol/sokol_gfx.h",
		}	
		
		includedirs {
							"..", -- For ImwConfig.h
							"../ImWindow",
							"../ImWindowEasyWindow",
							"../ImWindowSokol",
							"../Externals/imgui",
							"../Externals/EasyWindow",
							"../Externals/sokol",
							"../Externals/DirectX/include"
		}

		defines {
							"SOKOL_D3D11"
		}

		configuration		"x32"
			libdirs			"../Externals/DirectX/lib/x86"
			
		configuration		"x64"
			libdirs			"../Externals/DirectX/lib/x64"

		configuration		"Debug"
			flags			"Symbols"
			
		configuration		"Release"
			flags			"Optimize"

		SetupSuffix()
end
	
if _OPTIONS["with-bgfx"] then
		startproject "ImwWindowBGFX"
		project "ImwWindowBGFX"
			uuid				"DF12277C-C20B-4CD7-92D8-95ABD26986B1"
			kind				"ConsoleApp"
			targetdir			(PROJECT_RUNTIME_DIR)
			
			links {
								"ImWindow",
								"bgfx",
								"bimg",
								"bx"
			}
			files {
								"../sample.h",
								"../ImWindowEasyWindow/**.cpp",
								"../ImWindowEasyWindow/**.h",
								"../ImWindowBGFX/**.cpp",
								"../ImWindowBGFX/**.h",
								"../Externals/EasyWindow/EasyWindow*.cpp",
								"../Externals/EasyWindow/EasyWindow*.h",
			}
			
			includedirs {
								"..", -- For ImwConfig.h
								"../ImWindow",
								"../ImWindowEasyWindow",
								"../Externals/imgui",
								"../Externals/EasyWindow",

								path.join(BX_DIR, "include"),
								path.join(BX_DIR, "3rdparty"),
								path.join(BGFX_DIR, "include"),
								path.join(BGFX_DIR, "3rdparty"),
								path.join(BGFX_DIR, "examples/common"),
								--path.join(BGFX_DIR, "3rdparty/forsyth-too"),
								path.join(BIMG_DIR, "include"),
								path.join(BIMG_DIR, "3rdparty"),
			}

			configuration		"Debug"
				flags			{ "Symbols" }
				
			configuration		"Release"
				flags			{ "Optimize" }

			SetupSuffix()

			configuration {}

			configuration { "vs*" }
				buildoptions
				{
					"/wd 4127", -- Disable 'Conditional expression is constant' for do {} while(0).
					"/wd 4201", -- Disable 'Nonstandard extension used: nameless struct/union'. Used for uniforms in the project.
					"/wd 4345", -- Disable 'An object of POD type constructed with an initializer of the form () will be default-initialized'. It's an obsolete warning.
				}
				linkoptions
				{
					"/ignore:4199", -- LNK4199: /DELAYLOAD:*.dll ignored; no imports found from *.dll
				}
				links
				{ -- this is needed only for testing with GLES2/3 on Windows with VS2008
					"DelayImp",
				}

			configuration { "vs*", "x32" }
				links
				{
					"psapi",
				}

			configuration { "vs2010" }
				linkoptions
				{ -- this is needed only for testing with GLES2/3 on Windows with VS201x
					"/DELAYLOAD:\"libEGL.dll\"",
					"/DELAYLOAD:\"libGLESv2.dll\"",
				}
end
