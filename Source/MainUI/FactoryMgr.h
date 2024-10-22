#ifndef FACTORYMGR_H
#define FACTORYMGR_H
#include "..\pch.h"

#include <memory>
#include <string>
#include "..\PAUTFileReader\AscanDattype.hpp"
#include "MainUI/statuslogs.h"
#include <opencv2/opencv.hpp>
#include "MainUI/mainwindow.h"
#include "..\Source\event\ZoomableGraphicsView.h"
#include "OmConnect/CircularBuffer.h"
// forward declaration for fixing Circular reference
class nLogFrame;
class CviewFrame; // Oxy
class SviewFrame; // Oyz
class AviewFrame; // Oz
class BviewFrame; // Oxz


// Frame Factory
class nFactoryFrame {
public:
	static std::shared_ptr<nLogFrame> crLogFrm();
	static std::shared_ptr<CviewFrame> crCviewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<SviewFrame> crSViewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<AviewFrame> crAviewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<BviewFrame> crBviewFrm(nmainUI::UIFrame* uifmr);
};


#endif // FACTORYMGR_H
