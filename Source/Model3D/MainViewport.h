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
    VulkanWindow() : m_renderer(nullptr) {}

    QVulkanWindowRenderer* createRenderer() override
    {
        m_renderer = new TriangleRenderer(this, 1);
        return m_renderer;
    }

    TriangleRenderer* getRenderer() const { return m_renderer; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void GetDeviceInfo();
    QPoint lastMousePosition;
    bool isRightMouseButtonPressed = false;
    TriangleRenderer* m_renderer;
};
