#ifndef FACTORYMGR_H
#define FACTORYMGR_H
#include "pch.h"


///  Virtual Basic Frame 
/// </summary>

class nLogFrame;
class CviewFrame;
class SviewFrame;
class BviewFrame;
class IAFrame;  
// Frame Factory
class nFactoryFrame {
public:
	static std::shared_ptr<nLogFrame> crLogFrm();
	static std::shared_ptr<CviewFrame> crCviewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<SviewFrame> crSViewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<IAFrame> crAviewFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<IAFrame> crAvRealFrm(nmainUI::UIFrame* uifmr);
    static std::shared_ptr<BviewFrame> crBviewFrm(nmainUI::UIFrame* uifmr);
};

#endif // FACTORYMGR_H
