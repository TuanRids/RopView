#include "..\Source\Frames\nLogFrame.h"
#include "..\Source\Frames\CscanFrame.h"
#include "Frames/BscanFrame.h"
#include "FactoryMgr.h"
AscanData nFrame::scandat;
nmainUI::statuslogs* nFrame::sttlogs = nullptr;
curpt3d nFrame::curpt;
std::shared_ptr<nLogFrame> nFactoryFrame::createLogFrame() {
    return std::make_unique<nLogFrame>();
}

std::shared_ptr<CscanFrame> nFactoryFrame::createGraphicsFrame() {
    return std::make_unique<CscanFrame>();     
}

std::shared_ptr<BscanFrame> nFactoryFrame::createBscanFrame()
{    return std::make_unique<BscanFrame>();
}
