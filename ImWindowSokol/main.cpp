
#include "ImwWindowManagerSokol.h"

#include "../sample.h"

int main()
{
	PreInitSample();

	sg_desc oSokolDesc = { 0 };
	ImwWindowManagerSokol oMgr(&oSokolDesc, true);

	oMgr.Init();

	InitSample();

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	return 0;
}
