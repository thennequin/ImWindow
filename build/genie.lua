
-- Single file format
local generateSFF = false
if _ARGS[1] == "sff" then
	generateSFF = true;
end

if generateSFF == true then
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

solution "ImWindow"
	location				(_ACTION)
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x32", "x64" }
	objdir					("../Intermediate/".._ACTION)

	-- Inlude only SFF files in project
	if generateSFF == true then

		project "ImWindowSFF"
			uuid			"458E707F-2347-47D2-842A-A431CA538063"
			kind			"StaticLib"
			targetdir		"../Output/"

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
		targetdir			"../Output/"

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

	project "ImWindowDX11"
		uuid				"449C0C09-919A-4337-A09A-DFC2420A41B0"
		kind				"WindowedApp"
		targetdir			"../Output/"
		
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
