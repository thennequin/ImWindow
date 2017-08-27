
#include "bgfx/bgfx.h"
#include "ImwWindowManagerBGFX.h"
#include "../sample.h"

int main()
{
	PreInitSample();

	ImwWindowManagerBGFX oMgr(bgfx::RendererType::Direct3D11);

	oMgr.Init();

	InitSample();

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	ImGui::Shutdown();

	return 0;
}
