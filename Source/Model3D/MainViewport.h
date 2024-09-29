#pragma once
#include "..\pch.h"
#include <QVulkanWindow>
#include "MeshManager.h"
#include <QVulkanFunctions>
#include "..\MainUI\statuslogs.h"
class MeshRenderer;

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    VulkanWindow() : m_renderer(nullptr) {}

    QVulkanWindowRenderer* createRenderer() override
    {
        m_renderer = new MeshRenderer(this, 1);
        return m_renderer;
    }

    MeshRenderer* getRenderer() const { return m_renderer; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
private:
    void GetDeviceInfo();
    QPoint lastMousePosition;
    bool isRightMouseButtonPressed = false;
    MeshRenderer* m_renderer;
    float m_zoomLevel;
};
