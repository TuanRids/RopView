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
        QGraphicsView(parent), zoomFactor(1.15), isPanning(false) {
        setDragMode(QGraphicsView::NoDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        if (event->angleDelta().y() > 0) {
            // Zoom in
            scale(zoomFactor, zoomFactor);
        }
        else {
            // Zoom out
            scale(1.0 / zoomFactor, 1.0 / zoomFactor);
        }
    }
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
			isPanning = true;
			panStartPoint = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
        QGraphicsView::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent* event) override {
        if (isPanning) {
            QPointF delta = event->pos() - panStartPoint;
			panStartPoint = event->pos();
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
			verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        }
    }
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            isPanning = false;
            setCursor(Qt::ArrowCursor);  // Change cursor back to arrow
        }
        QGraphicsView::mouseReleaseEvent(event);
    }

private:
    const double zoomFactor;
    bool isPanning;  // Flag to indicate if panning is active
    QPoint panStartPoint;  // Starting point for panning

};

#endif // ZOOMABLEGRAPHICSVIEW_H
