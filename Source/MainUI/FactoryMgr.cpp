#include "..\pch.h"
#include "FactoryMgr.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"


std::shared_ptr<CviewFrame> nFactoryFrame::crCviewFrm() {
    return std::make_shared<CviewFrame>();
}

std::shared_ptr<SviewFrame> nFactoryFrame::crSViewFrm()
{
    return std::make_shared<SviewFrame>();

}

std::shared_ptr<AviewFrame> nFactoryFrame::crAviewFrm()
{
    return std::make_shared<AviewFrame>();
}

std::shared_ptr<BviewFrame> nFactoryFrame::crBviewFrm()
{
    return std::make_shared<BviewFrame>();
}
