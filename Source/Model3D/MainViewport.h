#pragma once
#include "..\pch.h"
#include <QVulkanWindow>
#include "TriangleTModel.h"
#include <QVulkanFunctions>
#include "..\MainUI\statuslogs.h"
class TriangleRenderer;

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    // auto generated
    QVulkanWindowRenderer* createRenderer() override {return new TriangleRenderer(this, 1); };
private:
    void GetDeviceInfo();
signals: // no need to declare for the signals
    void frameQueued(int colorValue);
};


