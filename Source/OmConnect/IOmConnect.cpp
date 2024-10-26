#include "../pch.h"
#include "IOmConnect.h"
#include "OmConnect.h"

IOmConnect::~IOmConnect() {}
std::shared_ptr<IOmConnect> IOmConnect::Create()
{
	static std::shared_ptr<OmConnect> om = std::make_shared<OmConnect>();
	if (om)
	{
		om.reset();
		om = std::make_shared<OmConnect>();
	}
	return om;
}