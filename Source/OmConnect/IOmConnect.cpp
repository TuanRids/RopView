#include "../pch.h"
#include "IOmConnect.h"
#include "OmConnect.h"

static std::shared_ptr<OmConnect> omc = nullptr;
atomic<bool> IOmConnect::isUpdate = false;
IOmConnect::~IOmConnect() {}
std::shared_ptr<IOmConnect> IOmConnect::Create()
{
	/*static std::shared_ptr<OmConnect> om = nullptr;
	if (om)
	{
		om->omDisconnectDevice();
		om.reset();
		om = std::make_shared<OmConnect>();
	}
	else 
		om = std::make_shared<OmConnect>();
	return om;*/
	if (!omc) omc = std::make_shared<OmConnect>();
	return omc;

}

void IOmConnect::ReleaseDevice()
{
	if (!omc) return;
	omc->omDisconnectDevice();
	omc.reset();
	omc = nullptr;
}
