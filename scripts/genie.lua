
newoption {
	trigger = "with-sff",
	description = "Enable generation of single file format",
}

newoption {
	trigger = "with-dx11",
	description = "Enable DX11 backend sample",
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
							"../Externals/imgui/SFF_rect_pack.h",
							"../Externals/imgui/SFF_textedit.h",
							"../Externals/imgui/SFF_truetype.h",
			}
			
			vpaths {
							["ImGui"] = "../Externals/imgui/**"
			}
			
			includedirs {
							"../Externals/imgui",
							"../ImWindow/"
			}
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
							"../Externals/imgui/SFF_rect_pack.h",
							"../Externals/imgui/SFF_textedit.h",
							"../Externals/imgui/SFF_truetype.h",
		}
		
		vpaths {
							["ImGui"] = "../Externals/imgui/**"
		}
		
		includedirs {
							"../Externals/imgui",
							"../ImWindow/"
		}
		
		configuration		"Debug"
			targetsuffix	"_d"
			flags			{ "Symbols" }
			
		configuration		"Release"
			targetsuffix	"_r"
			flags			{ "Optimize" }

if _OPTIONS["with-dx11"] then
	startproject "ImWindowDX11"
	project "ImWindowDX11"
		uuid				"449C0C09-919A-4337-A09A-DFC2420A41B0"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)
		
		links				{ "ImWindow" }
		files {
							"../ImWindowDX11/**.cpp",
							"../ImWindowDX11/**.h",
							"../Externals/imgui/examples/directx11_example/imgui_impl_dx11.cpp"
		}	
		
		includedirs {
							"../ImWindow",
							"../Externals/imgui",
							"../Externals/imgui/examples/directx11_example",
							"../Externals/DirectX/include"
		}
		
		
		platforms			"x32"
			libdirs {
							"../Externals/DirectX/lib/x86"
			}
			
		platforms			"x64"
			libdirs {
							"../Externals/DirectX/lib/x64"
			}

		configuration		"Debug"
			targetsuffix	"_d"
			flags			{ "Symbols" }
			
		configuration		"Release"
			targetsuffix	"_r"
			flags			{ "Optimize" }
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
								"../ImWindowBGFX/**.cpp",
								"../ImWindowBGFX/**.h",
			}
			
			includedirs {
								"../ImWindow",
								"../Externals/imgui",

								path.join(BX_DIR, "include"),
								path.join(BX_DIR, "3rdparty"),
								path.join(BGFX_DIR, "include"),
								path.join(BGFX_DIR, "3rdparty"),
								path.join(BGFX_DIR, "examples/common"),
								--path.join(BGFX_DIR, "3rdparty/forsyth-too"),
								path.join(BIMG_DIR, "include"),
								path.join(BIMG_DIR, "3rdparty"),
			}		
			
			platforms			"x32"
				libdirs {
								--"../Externals/glfw-3.2/lib-vc2015/x86"
				}
				
			platforms			"x64"
				libdirs {
								--"../Externals/glfw-3.2/lib-vc2015/x64"
				}

			configuration		"Debug"
				targetsuffix	"_d"
				flags			{ "Symbols" }
				
			configuration		"Release"
				targetsuffix	"_r"
				flags			{ "Optimize" }

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