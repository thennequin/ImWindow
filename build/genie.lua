
local generateSTB = false
if _ARGS[1] == "stb" then
	generateSTB = true;
end

if generateSTB == true then
	print "=================="
	print "Generate STB files"
	print "=================="

	local EOL = "\n"

	function IncludeSTB( inputFileName, outFile )
		local inputFile = io.open(inputFileName, "r")
		if nil == inputFile then
			print("Error: Can't include file " .. inputFileName)
		else
			print("Include file " .. inputFileName)
			local isInSTBArea = false
			while true do
				local line = inputFile:read()
				if line == nil then
					break
				elseif line == "//STB_BEGIN" then
					isInSTBArea = true
				elseif line == "//STB_END" then 
					isInSTBArea = false
				elseif isInSTBArea == true then
					outFile:write(line..EOL);
				end
			end

			outFile:write(EOL);
		end
	end

	-- Write header
	local fileStbHeader = io.open("../ImWindow.h","w+")

	-- Write informations
	fileStbHeader:write("// ImWindow"..EOL)

	fileStbHeader:write("#include \"ImwConfig.h\""..EOL..EOL)

	fileStbHeader:write("namespace ImWindow {"..EOL)
	--Forward declare classes
	fileStbHeader:write("	class ImWindow;"..EOL)
	fileStbHeader:write("	class ImContainer;"..EOL)
	fileStbHeader:write("	class ImPlatformWindow;"..EOL)
	fileStbHeader:write("	class ImWindowManager;"..EOL)
	fileStbHeader:write(EOL)
	IncludeSTB( "../ImWindow/ImwWindow.h", fileStbHeader )
	IncludeSTB( "../ImWindow/ImwContainer.h", fileStbHeader )
	IncludeSTB( "../ImWindow/ImwPlatformWindow.h", fileStbHeader )
	IncludeSTB( "../ImWindow/ImwWindowManager.h", fileStbHeader )
	fileStbHeader:write("}\n")
	fileStbHeader:close()

	-- Write source
	local fileStbSource = io.open("../ImWindow.cpp","w+")

	fileStbSource:write("#include \"ImWindow.h\""..EOL..EOL)

	fileStbSource:write("namespace ImWindow {"..EOL)
	IncludeSTB( "../ImWindow/ImwWindow.cpp", fileStbSource )
	IncludeSTB( "../ImWindow/ImwContainer.cpp", fileStbSource )
	IncludeSTB( "../ImWindow/ImwPlatformWindow.cpp", fileStbSource )
	IncludeSTB( "../ImWindow/ImwWindowManager.cpp", fileStbSource )
	fileStbSource:write("}"..EOL)
	fileStbSource:close()

	print "=================="
end

solution "ImWindow"
	location				(_ACTION)
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x32", "x64" }
	objdir					("../Intermediate/".._ACTION)

	-- Inlude only STB files in project
	if generateSTB == true then

		project "ImWindowSTB"
			uuid			"458E707F-2347-47D2-842A-A431CA538063"
			kind			"StaticLib"
			targetdir		"../Output/"

			files {
							"../ImWindow.cpp",
							"../ImWindow.h",
							"../ImWindow/ImwConfig.h",
							"../Externals/ImGui/imgui/imconfig.h",
							"../Externals/ImGui/imgui/imgui.h",
							"../Externals/ImGui/imgui/imgui_internal.h",
							"../Externals/ImGui/imgui/imgui.cpp",
							"../Externals/ImGui/imgui/imgui_draw.cpp",
							"../Externals/ImGui/imgui/stb_rect_pack.h",
							"../Externals/ImGui/imgui/stb_textedit.h",
							"../Externals/ImGui/imgui/stb_truetype.h",
			}
			
			vpaths {
							["ImGui"] = "../Externals/ImGui/imgui/**"
			}
			
			includedirs {
							"../Externals/ImGui",
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
							"../Externals/ImGui/imgui/imconfig.h",
							"../Externals/ImGui/imgui/imgui.h",
							"../Externals/ImGui/imgui/imgui_internal.h",
							"../Externals/ImGui/imgui/imgui.cpp",
							"../Externals/ImGui/imgui/imgui_draw.cpp",
							"../Externals/ImGui/imgui/stb_rect_pack.h",
							"../Externals/ImGui/imgui/stb_textedit.h",
							"../Externals/ImGui/imgui/stb_truetype.h",
		}
		
		vpaths {
							["ImGui"] = "../Externals/ImGui/imgui/**"
		}
		
		includedirs {
							"../Externals/ImGui",
							"../ImWindow/"
		}
		
		configuration		"Debug"
			targetsuffix	"_d"
			
		configuration		"Release"
			targetsuffix	"_r"

	project "ImWindowDX11"
		uuid				"449C0C09-919A-4337-A09A-DFC2420A41B0"
		kind				"WindowedApp"
		targetdir			"../Output/"
		
		links				{ "ImWindow" }
		files {
							"../ImWindowDX11/**.cpp",
							"../ImWindowDX11/**.h",
							"../Externals/ImGui/imgui/examples/directx11_example/imgui_impl_dx11.cpp"
		}	
		
		includedirs {
							"../ImWindow",
							"../Externals/ImGui",
							"../Externals/ImGui/imgui",
							"../Externals/ImGui/imgui/examples/directx11_example",
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