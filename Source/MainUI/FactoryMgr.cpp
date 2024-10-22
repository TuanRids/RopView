#include "..\Source\Frames\nLogFrame.h"
#include "..\Source\Frames\CviewFrame.h"
#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"
#include "FactoryMgr.h"

//AscanData nFrame::scandat;
//nmainUI::statuslogs* nFrame::sttlogs = nullptr;
//curpt3d nFrame::curpt { 0,0,0 };
//bool nFrame::isPanning = false;
//CircularBuffer<std::vector<int>> nFrame::sharedBuffer = CircularBuffer<std::vector<int>>(1000);

std::shared_ptr<nLogFrame> nFactoryFrame::crLogFrm() {
    return std::make_unique<nLogFrame>();
}

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
