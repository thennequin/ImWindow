
#include "ImwWindowManagerSokol.h"

#include "../sample.h"

int main()
{
	PreInitSample();

	ImwWindowManagerSokol oMgr;

	oMgr.Init();

	InitSample();

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	ImGui::Shutdown();

	return 0;
}
