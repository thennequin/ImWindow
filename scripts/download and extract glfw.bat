@echo off

if not exist "%~dp0\glfw-3.2.1.bin.WIN32.zip" (
	echo Downloading GLFW 3.2.1 Win32...
	powershell [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12;Invoke-WebRequest -Uri "https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN32.zip" -OutFile '%~dp0\glfw-3.2.1.bin.WIN32.zip'
)
if not exist "%~dp0\glfw-3.2.1.bin.WIN64.zip" (
	echo Downloading GLFW 3.2.1 WIN64...
	powershell [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12;Invoke-WebRequest -Uri "https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN64.zip" -OutFile '%~dp0\glfw-3.2.1.bin.WIN64.zip'
)

echo Extracting GLFW 3.2.1 Win32...
powershell Expand-Archive '%~dp0\glfw-3.2.1.bin.WIN32.zip' -DestinationPath '%~dp0\..\Externals\'
echo Extracting GLFW 3.2.1 Win64...
powershell Expand-Archive '%~dp0\glfw-3.2.1.bin.WIN64.zip' -DestinationPath '%~dp0\..\Externals\'

echo Finish
