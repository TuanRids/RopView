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

struct curpt3d 
{ 
    int x{ -1 }, y{ -1 }, z{ -1 };
    bool CheckIdx(int nx, int ny, int nz);
};


class nFrame {
public:
	virtual QWidget* createFrame() = 0;
	virtual ~nFrame() = default;	
    void setScandat(const AscanData& dataa) { scandat = dataa; }
    void clearScandat() { scandat = AscanData(); }
    void setSttlogs() { if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); } }

protected:
    static bool isPanning ;
    static curpt3d curpt;
	static AscanData scandat;
    static nmainUI::statuslogs* sttlogs ;   
    static CircularBuffer<std::vector<int>> sharedBuffer;
    // TODO optimize this function later
    void UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene, std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor);
    std::vector<Color> CreateColorPalette();

};

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
