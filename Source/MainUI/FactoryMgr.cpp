#include "..\Source\Frames\nLogFrame.h"
#include "..\Source\Frames\CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "Frames/AscanFrame.h"
#include "FactoryMgr.h"

AscanData nFrame::scandat;
nmainUI::statuslogs* nFrame::sttlogs = nullptr;
curpt3d nFrame::curpt { 0,0,0 };

std::shared_ptr<nLogFrame> nFactoryFrame::crLogFrm() {
    return std::make_unique<nLogFrame>();
}

std::shared_ptr<CscanFrame> nFactoryFrame::crCscanFrm(nmainUI::UIFrame* uifmr) {
    auto res = std::make_unique<CscanFrame>();
    res->setUIFrame(uifmr);
    return res;     
}

std::shared_ptr<BscanFrame> nFactoryFrame::crBscanFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<BscanFrame>();
    res->setUIFrame(uifmr);
    return res;

}

std::shared_ptr<AscanFrame> nFactoryFrame::crAscanFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<AscanFrame>();
    res->setUIFrame(uifmr);
    return res;
}
