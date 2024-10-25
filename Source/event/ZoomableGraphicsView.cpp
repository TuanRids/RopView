#include "ZoomableGraphicsView.h"

ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
    : QGraphicsView(parent), zoomFactor(1.15), isLDragging(false), isPanning(false), navigatorView(nullptr) {
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void ZoomableGraphicsView::setNavigator(QGraphicsView* navigatorView) {
    this->navigatorView = navigatorView;
}

void ZoomableGraphicsView::wheelEvent(QWheelEvent* event) {
    QPointF scenePosBeforeZoom = mapToScene(event->position().toPoint());

    if (event->angleDelta().y() > 0) {
        scale(zoomFactor, zoomFactor);
    }
    else {
        QRectF viewRect = mapToScene(rect()).boundingRect();

        QGraphicsItem* artwork = nullptr;
        for (auto* item : scene()->items()) {
            if (item->data(0).toString() == "artwork") {
                artwork = item;
                break;
            }
        }

        if (artwork) {
            QRectF artworkRect = artwork->boundingRect();
            if (viewRect.width() > artworkRect.width() || viewRect.height() > artworkRect.height()) {
                return;
            }
            scale(1.0 / zoomFactor, 1.0 / zoomFactor);
        }
    }

    QPointF scenePosAfterZoom = mapToScene(event->position().toPoint());
    QPointF delta = scenePosBeforeZoom - scenePosAfterZoom;
}

void ZoomableGraphicsView::leaveEvent(QEvent* event) {
    emit mouseLeftView();
    QGraphicsView::leaveEvent(event);
}

void ZoomableGraphicsView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        emit mouseClicked(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
    }
    if (event->button() == Qt::MiddleButton) {
        isPanning = true;
        panStartPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ZoomableGraphicsView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::MiddleButton) {
        isPanning = true;
        QPointF delta = event->pos() - panStartPoint;
        panStartPoint = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
    else if (event->buttons() == Qt::LeftButton) {
        isLDragging = true;
        QPointF scenePos = mapToScene(event->pos());
        emit mouseDragClicked(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
    }

    QPointF scenePos = mapToScene(event->pos());
    emit mouseMoved(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
}

void ZoomableGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    if (event->button() == Qt::LeftButton && isLDragging) {
        isLDragging = false;
        emit mouseStopDragClicked();
    }
}

void ZoomableGraphicsView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_T) 
    {
        emit nKeyPressedEvent();
    }

}
