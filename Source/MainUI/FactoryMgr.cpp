#include "Frames\nLogFrame.h"
#include "Frames\CviewFrame.h"
#include "Frames\SviewFrame.h"
#include "Frames\AviewFrame.h"
#include "Frames\BviewFrame.h"
#include "IFrames\IAFrame.h"

#include "Frames/SviewFrame.h"
#include "Frames/AviewFrame.h"
#include "Frames/BviewFrame.h"
#include "FactoryMgr.h"
#include "SettingPara.h"
std::shared_ptr<nLogFrame> nFactoryFrame::crLogFrm() {
    return std::make_shared<nLogFrame>();
}

std::shared_ptr<CviewFrame> nFactoryFrame::crCviewFrm(nmainUI::UIFrame* uifmr) {
    auto res = std::make_shared<CviewFrame>();
    res->setUIFrame(uifmr);
    return res;     
}

std::shared_ptr<SviewFrame> nFactoryFrame::crSViewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_shared<SviewFrame>();
    res->setUIFrame(uifmr);
    return res;

}

std::shared_ptr<IAFrame> nFactoryFrame::crAviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_shared<AviewFrame>();  
    res->setUIFrame(uifmr);
    return res;

}

std::shared_ptr<IAFrame> nFactoryFrame::crAvRealFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_shared<AvRealFrame>(); 
    res->setUIFrame(uifmr);
    return res;
}

std::shared_ptr<BviewFrame> nFactoryFrame::crBviewFrm(nmainUI::UIFrame* uifmr)
{
    auto res = std::make_unique<BviewFrame>();
    res->setUIFrame(uifmr);
    return res;
}

void nFrame::setScandat(const AscanData& dataa)
{
    scandat = dataa; 
}

void nFrame::clearScandat()
{ scandat = AscanData(); }

void nFrame::UpdateGraphic(std::shared_ptr<cv::Mat> OrgImg, std::shared_ptr<cv::Mat> Img, std::shared_ptr<QGraphicsScene> scene,
    std::shared_ptr<ZoomableGraphicsView> graphicsView, int res, Qt::GlobalColor xcolor, Qt::GlobalColor ycolor)
{
      
    double frameRatio = static_cast<double>(graphicsView->size().width()) / static_cast<double>(graphicsView->size().height());
    double imageRatio = static_cast<double>(OrgImg->cols) / static_cast<double>(OrgImg->rows);
    
    auto newWidth = (frameRatio > imageRatio) ? static_cast<int>(OrgImg->rows * frameRatio) : OrgImg->cols;
    auto newHeight = (frameRatio > imageRatio) ? OrgImg->rows : static_cast<int>(OrgImg->cols / frameRatio);
    
    auto scaleFactor = (!isPanning || SettingsManager::getInstance()->getSettings().bhighResBscan) ? res : 1.0;
    cv::resize(*OrgImg, *Img, cv::Size(newWidth * scaleFactor, newHeight * scaleFactor), 0, 0, cv::INTER_LINEAR);
    
    cv::GaussianBlur(*Img, *Img, cv::Size(1, 1), 0);
    auto qImage = std::make_shared<QImage>(Img->data, Img->cols, Img->rows, Img->step, QImage::Format_RGB888);
    *qImage = qImage->rgbSwapped();
    
    QPixmap pixmap = QPixmap::fromImage(*qImage);
    scene->clear();
    QGraphicsPixmapItem* artworkItem = scene->addPixmap(pixmap);
    artworkItem->setData(0, "artwork");
    
    int gridSpacing = 20;
    QPen gridx(xcolor);
    QPen gridy(ycolor);
    gridx.setStyle(Qt::SolidLine); gridx.setWidth(0.003 * std::min(scene->height(), scene->width()));
    gridy.setStyle(Qt::SolidLine); gridy.setWidth(0.003 * std::min(scene->height(), scene->width()));

    QGraphicsLineItem* verticalLine = scene->addLine(-2, 0 , -2, newHeight * scaleFactor + 4, gridx);
    

    QGraphicsLineItem* horizontalLine = scene->addLine( - 4 , newHeight * scaleFactor + 2, newWidth*scaleFactor, newHeight * scaleFactor + 2, gridy);
    
    
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    graphicsView->update();
}
