#ifndef ASCANFRAME_H
#define ASCANFRAME_H

#include "pch.h"
#include "..\..\Source\PAUTFileReader\AscanProcessor.h"
#include "..\..\Source\MainUI\ObserverMgr.h"
#include "..\..\Source\MainUI\mainwindow.h"
#include "IFrames/IAFrame.h"
// Aview Frame
class AviewFrame : public IAFrame, public nObserver {
private:
    //************** Method
    void CreateAview() override;

    //************** Properties
    unsigned int y_level_ = 0;
    unsigned int x_level_ = 0;
    

public:
    void update() override;
};

class AvRealFrame : public IAFrame, public nObserver {
private:
    void CreateAview() override {
        if (!scene || !graphicsView) return;
        scene->clear();
        QGraphicsTextItem* text = scene->addText("Real-time Mode");
        text->setDefaultTextColor(Qt::green);
    }
public:
    void update() override {
        CreateAview(); 
    }
};

#endif
