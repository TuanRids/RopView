#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QScrollBar>

class ZoomableGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ZoomableGraphicsView(QWidget* parent = nullptr) :
        QGraphicsView(parent), zoomFactor(1.15), isLDragging(false) {
        setDragMode(QGraphicsView::NoDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    void setNavigator(QGraphicsView* navigatorView) {
        this->navigatorView = navigatorView;
    }

signals:
        void mouseClicked(int x, int y);
        void mouseDragClicked(int x, int y);
        void mouseStopDragClicked();
        void mouseMoved(int x, int y);
protected:
    
    void wheelEvent(QWheelEvent* event) override {
        QPointF scenePosBeforeZoom = mapToScene(event->position().toPoint());

        if (event->angleDelta().y() > 0) {
            scale(zoomFactor, zoomFactor);
        }
        else {
            // Zoom out
            QRectF viewRect = mapToScene(rect()).boundingRect();
            QRectF sceneRect = scene()->itemsBoundingRect();
            if (viewRect.width() < sceneRect.width() || viewRect.height() < sceneRect.height()) {
                scale(1.0 / zoomFactor, 1.0 / zoomFactor);
            }
        }

        QPointF scenePosAfterZoom = mapToScene(event->position().toPoint());

        QPointF delta = scenePosBeforeZoom - scenePosAfterZoom;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }


    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            emit mouseClicked(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
        }
        if (event->button() == Qt::MiddleButton) {
            isPanning = true;
            panStartPoint = event->pos();
            setCursor(Qt::ClosedHandCursor);  // Change cursor for panning
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() == Qt::MiddleButton) {
            // Panning logic
            isPanning = true;
            QPointF delta = event->pos() - panStartPoint;
            panStartPoint = event->pos();
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        }
        else if (event->buttons()== Qt::LeftButton) {
            // Left-click dragging
            isLDragging = true;
            QPointF scenePos = mapToScene(event->pos());
            emit mouseDragClicked(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
        }

        // Always emit mouse move event
        QPointF scenePos = mapToScene(event->pos());
        emit mouseMoved(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            // Stop panning
            isPanning = false;
            setCursor(Qt::ArrowCursor);  // Restore default cursor
        }
        if (event->button() == Qt::LeftButton && isLDragging) {
            // Stop dragging
            isLDragging = false;
            emit mouseStopDragClicked();
        }
    }

private:
    const double zoomFactor;
    bool isPanning;  // Track if panning
    bool isLDragging;  // Track if dragging with left button
    QPoint panStartPoint;  // Starting point for panning
    QGraphicsView* navigatorView = nullptr;
    void updateNavigator() {
        //if (navigatorView) {
        //    // Get the current visible center area in the main view
        //    QPointF mainViewCenter = mapToScene(viewport()->rect().center());

        //    // Create a point (ellipse) at the current center
        //    QGraphicsEllipseItem* pointItem = new QGraphicsEllipseItem(mainViewCenter.x() - 5, mainViewCenter.y() - 5, 10, 10);
        //    pointItem->setBrush(Qt::red);  // Set point color
        //    pointItem->setPen(Qt::NoPen);  // Remove border for cleaner look

        //    // Add the point to the scene (do not clear the scene to keep existing content)
        //    scene()->addItem(pointItem);

        //    // Adjust the view on the navigator to show the full scene
        //    navigatorView->fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        //}
    }


};

#endif // ZOOMABLEGRAPHICSVIEW_H
