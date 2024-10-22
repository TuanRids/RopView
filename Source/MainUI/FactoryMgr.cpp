#include "..\pch.h"
#include "FactoryMgr.h"
#include "Frames/CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"


std::shared_ptr<CviewFrame> nFactoryFrame::crCviewFrm(nmainUI::UIFrame* uifmr) {
    auto res = std::make_unique<CviewFrame>();
    res->setUIFrame(uifmr);
    return res;     
}

std::shared_ptr<SviewFrame> nFactoryFrame::crSViewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<SviewFrame>();
    res->setUIFrame(uifmr);
    return res;

}

std::shared_ptr<AviewFrame> nFactoryFrame::crAviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<AviewFrame>();
    res->setUIFrame(uifmr);
    return res;
}

std::shared_ptr<BviewFrame> nFactoryFrame::crBviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<BviewFrame>();
    res->setUIFrame(uifmr);
    return res;
}
