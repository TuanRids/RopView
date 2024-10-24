#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QScrollBar>

class ZoomableGraphicsView
    : public QGraphicsView {
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
    void mouseLeftView();
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
    void leaveEvent(QEvent* event) override {
        emit mouseLeftView();
        QGraphicsView::leaveEvent(event);
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
    bool isPanning;             // Track if panning
    bool isLDragging;           // Track if dragging with left button
    QPoint panStartPoint;       // Starting point for panning
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

class XYOverlayGrid : public QObject, public QGraphicsItemGroup {
    QGraphicsScene* scene;
    QGraphicsView* graphicsView;
    QTimer* blinkTimer;
    bool isBlinkOn;           
public:
    XYOverlayGrid(QGraphicsView* ggraphicview, QGraphicsScene* scene) : graphicsView(ggraphicview), scene(scene), blinkTimer(nullptr), isBlinkOn(true) {
        blinkTimer = new QTimer(this);
        connect(blinkTimer, &QTimer::timeout, this, &XYOverlayGrid::toggleBlink);
        blinkTimer->start(100);
    }
    void updateOverlay(int original_y, int original_z, int imageWidth, int imageHeight) {
        removeItemsByData("VerHorLine");
        auto lineGroup = new QGraphicsItemGroup();
        scene->addItem(lineGroup);


        QGraphicsItem* artwork = nullptr;
        auto pointGroup = new QGraphicsItemGroup();
        scene->addItem(pointGroup);
        for (auto* item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                artwork = item;
                break;
            }
        }
        double artworkWidth = 0;
        double artworkHeight = 0;
        if (artwork) {
            QRectF artworkRect = artwork->boundingRect();
            artworkWidth = artworkRect.width();
            artworkHeight = artworkRect.height();
        }

        double viewWidth = graphicsView->viewport()->width();
        double viewHeight = graphicsView->viewport()->height();

        double scaleX = artworkWidth / viewWidth;
        double scaleY = artworkHeight / viewHeight;

        double pointSizeInScreen = 0.01 * std::min(viewWidth, viewHeight);
        double pointSizeInScene = pointSizeInScreen * std::max(scaleX, scaleY);

        QPen verticalPen(Qt::darkBlue);
        verticalPen.setWidth(pointSizeInScene * 0.1);
        QPen horizontalPen(Qt::magenta);
        horizontalPen.setWidth(pointSizeInScene * 0.1);

        auto verticalLine = new QGraphicsLineItem(original_y, 0, original_y, imageHeight);
        verticalLine->setData(0,"VerHorLine");
        verticalLine->setZValue(1);
        verticalLine->setPen(verticalPen);
        lineGroup->addToGroup(verticalLine);

        auto horizontalLine = new QGraphicsLineItem(0, original_z, imageWidth, original_z);
        horizontalLine->setData(0, "VerHorLine");
        horizontalLine->setZValue(1);
        horizontalLine->setPen(horizontalPen);
        lineGroup->addToGroup(horizontalLine);
    }
    void updatePoints(double pixelX, double pixelY, Qt::GlobalColor vcolor, Qt::GlobalColor hcolor) {
        removeItemsByData("Point");
        removeItemsByData("MarkLine");

        QGraphicsItem* artwork = nullptr;
        auto pointGroup = new QGraphicsItemGroup();
        scene->addItem(pointGroup);
        for (auto* item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                artwork = item;
                break;
            }
        }
        double artworkWidth = 0;
        double artworkHeight = 0;
        if (artwork) {
            QRectF artworkRect = artwork->boundingRect();
            artworkWidth = artworkRect.width();
            artworkHeight = artworkRect.height();
        }

        QPen pen(Qt::red);
        QBrush brush(Qt::red);
        double viewWidth = graphicsView->viewport()->width();
        double viewHeight = graphicsView->viewport()->height();

        double scaleX = artworkWidth / viewWidth;
        double scaleY = artworkHeight / viewHeight;

        double pointSizeInScreen = 0.01 * std::min(viewWidth, viewHeight); 
        double pointSizeInScene = pointSizeInScreen * std::max(scaleX, scaleY);  

        QPointF scenePoint = graphicsView->mapToScene(QPoint(static_cast<int>(pixelX), static_cast<int>(pixelY)));

        QGraphicsEllipseItem* point = new QGraphicsEllipseItem(pixelX - pointSizeInScene / 2, pixelY - pointSizeInScene / 2, pointSizeInScene, pointSizeInScene);
        point->setData(0, "Point");
        point->setZValue(2);  
        pen.setCosmetic(true);  
        point->setPen(pen);
        point->setBrush(brush);
        pointGroup->addToGroup(point);

        auto MarkLine = new QGraphicsItemGroup();
        scene->addItem(MarkLine);
        
        
        double lineSize = 0.001 * std::min(scene->height(), scene->width());

        auto* verticalLine = new QGraphicsLineItem(pixelX, 0, pixelX, artworkHeight);
        verticalLine->setData(0, "MarkLine");
        QPen gpen(vcolor);
        gpen.setWidth(pointSizeInScene * 0.1);
        gpen.setCosmetic(true);
        verticalLine->setPen(gpen);
        MarkLine->addToGroup(verticalLine);
        if (hcolor != Qt::color0)
        {
            QPen gpen(hcolor);
            gpen.setWidth(pointSizeInScene * 0.1);
            gpen.setCosmetic(true);
            auto* HorizontalLine = new QGraphicsLineItem(0, pixelY, artworkWidth, pixelY);
            HorizontalLine->setData(0, "MarkLine");
            HorizontalLine->setPen(gpen);
            MarkLine->addToGroup(HorizontalLine);
        };

    }


    void toggleBlink() {
        static const QVector<QColor> colors = { Qt::red, Qt::darkGreen, Qt::darkMagenta, Qt::yellow, Qt::cyan };
        static int currentColorIndex = 0;

        static int lineGroupBlinkCounter = 0;
        static bool isDashLine = false;

        isBlinkOn = !isBlinkOn;

        auto items = scene->items();
        for (auto* item : items) {
            if (item->data(0).toString() == "Point") {
                if (isBlinkOn) {
                    if (auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
                        QPen pen = ellipse->pen();
                        pen.setColor(colors[currentColorIndex]);
                        ellipse->setPen(pen);

                        QBrush brush = ellipse->brush();
                        brush.setColor(colors[currentColorIndex]);
                        ellipse->setBrush(brush);

                        currentColorIndex = (currentColorIndex + 1) % colors.size();
                    }
                    item->show();
                }
                else {
                    item->hide();
                }
            }
        }
        lineGroupBlinkCounter++;
        if (lineGroupBlinkCounter >= 5) {
            lineGroupBlinkCounter = 0;
            isDashLine = !isDashLine;

            for (auto* item : items) {
                if (item->data(0).toString() == "VerHorLine") {
                    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
                        QPen pen = lineItem->pen();
                        if (isDashLine) {
                            pen.setStyle(Qt::DashLine);
                        }
                        else {
                            pen.setStyle(Qt::SolidLine);
                        }
                        lineItem->setPen(pen);
                    }
                    item->show();
                }
            }
        }
        scene->update();      
    }

    void ClearLineGroup() {
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "VerHorLine") {
                scene->removeItem(item);
                delete item;
                break;
            }
        }
    }
    void removeItemsByData(const QString& dataName) {
        auto items = scene->items();
        for (auto* item : items) {
            if (item->data(0).toString() == dataName) {
                scene->removeItem(item);
                delete item;
            }
        }
    }
};
#endif // ZOOMABLEGRAPHICSVIEW_H
