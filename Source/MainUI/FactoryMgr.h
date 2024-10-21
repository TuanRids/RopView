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

/// <summary>
///  Virtual Basic Frame 
/// </summary>

// forward declaration for fixing Circular reference
class nLogFrame;
class CviewFrame; // Oxy
class SviewFrame; // Oyz
class AviewFrame; // Oz
class BviewFrame; // Oxz

struct curpt3d 
{ 
    int x{ -1 }, y{ -1 }, z{ -1 };
    bool CheckIdx(int nx, int ny, int nz)
    {
        if (nx != this->x && nx != -1) { this->x = nx;  return true; }
        if (ny != this->y && ny != -1) { this->y = ny; return true; }
        if (nz != this->z && nz != -1) { this->z = nz;  return true; }
		return false;
    }
};

class nFrame {
public:
	virtual QWidget* createFrame() = 0;
	virtual ~nFrame() = default;	
    void setScandat(const AscanData& dataa) { scandat = dataa; }
    void clearScandat() { scandat = AscanData(); }
    void setSttlogs() { if (!sttlogs) { sttlogs = &nmainUI::statuslogs::getinstance(); } }
    void UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene, 
        std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor);

protected:
    static bool isPanning ;
    static curpt3d curpt;
	static AscanData scandat;
    static nmainUI::statuslogs* sttlogs ;
    std::vector<Color> CreateColorPalette() {
        int gainFactor = 2;
        // Define a set of key colors that will be used as reference points for the gradient.
        std::vector<Color> colors;
        std::vector<Color> everyColors;

        // Key colors in the gradient, ranging from white to deep red.
        colors.emplace_back(Color{ 255, 255, 255 }); // white
        colors.emplace_back(Color{ 184, 212, 244 }); // light blue
        colors.emplace_back(Color{ 113, 170, 233 }); // blue
        colors.emplace_back(Color{ 62, 105, 190 });  // dark blue
        colors.emplace_back(Color{ 14, 37, 143 });   // dark purple
        colors.emplace_back(Color{ 27, 72, 129 });   // purple
        colors.emplace_back(Color{ 59, 140, 127 });  // green
        colors.emplace_back(Color{ 126, 187, 94 });  // light green
        colors.emplace_back(Color{ 211, 223, 45 });  // yellow
        colors.emplace_back(Color{ 241, 211, 43 });  // light yellow
        colors.emplace_back(Color{ 222, 156, 80 });  // orange
        colors.emplace_back(Color{ 209, 121, 87 });  // light orange
        colors.emplace_back(Color{ 205, 116, 49 });  // dark orange
        colors.emplace_back(Color{ 194, 98, 23 });   // brown
        colors.emplace_back(Color{ 167, 50, 26 });   // dark brown
        colors.emplace_back(Color{ 145, 12, 29 });   // red

        // Number of interpolation points between each pair of key colors.
        size_t interpolationPoints{ 7 };
        double f = 1.0 / interpolationPoints;   // Fractional step size for interpolation.

        // Interpolate between each pair of consecutive key colors.
        for (size_t colorIdx(0); colorIdx < colors.size() - 1; colorIdx++) {
            Color x = colors[colorIdx];         // Start color
            Color y = colors[colorIdx + 1];     // End color

            // Generate intermediate colors between x and y.
            for (size_t j(0); j < interpolationPoints; j++) {
                // Interpolate each RGB component separately.
                uint8_t r = static_cast<uint8_t>(x.R + (y.R - x.R) * (j * f));
                uint8_t g = static_cast<uint8_t>(x.G + (y.G - x.G) * (j * f));
                uint8_t b = static_cast<uint8_t>(x.B + (y.B - x.B) * (j * f));

                // Add the interpolated color to the resulting palette.
                everyColors.emplace_back(Color{ r, g, b });
            }
        }

        // Apply gainFactor to shift colors based on the intensity
        size_t numColors = everyColors.size();
        std::vector<Color> gainAdjustedColors(numColors);

        // Adjust the colors based on the gain factor.
        for (size_t i = 0; i < numColors; ++i) {
            // Calculate the shifted index based on gain.
            int shiftedIndex = static_cast<int>(i * gainFactor);
            if (shiftedIndex >= numColors) {
                shiftedIndex = static_cast<int>(numColors - 1); // Cap at the last color
            }
            gainAdjustedColors[i] = everyColors[shiftedIndex];
        }

        return gainAdjustedColors;

    }
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
