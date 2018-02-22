@echo off

if not exist "genie.exe" (
	echo Downloading genie...
	rem bitsadmin /transfer "genie" "https://github.com/bkaradzic/bx/raw/master/tools/bin/windows/genie.exe" "%~dp0\genie.exe"
	powershell [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri "https://github.com/bkaradzic/bx/raw/master/tools/bin/windows/genie.exe" -OutFile '%~dp0\genie.exe'
)

IF "%1"=="" (
	echo No argument set
	echo Exemple : build.bat vs2015
	exit 1
)

genie.exe %*
