
#include "ImwWindowManagerDX11.h"
#include "../sample.h"

int main()
{
	PreInitSample();

	ImwWindowManagerDX11 oMgr;

	oMgr.Init();

	InitSample();

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	ImGui::Shutdown();

	return 0;
}
