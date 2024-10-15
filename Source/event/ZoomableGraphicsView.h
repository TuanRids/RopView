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
    QGraphicsItemGroup* lineGroup;
    QGraphicsItemGroup* pointGroup;
    QTimer* blinkTimer;
    bool isBlinkOn;             // Flag to track whether to show the lines or not
    QGraphicsLineItem* verticalLine;
    QGraphicsLineItem* horizontalLine;
public:
    XYOverlayGrid(QGraphicsScene* scene) : scene(scene),lineGroup(nullptr), blinkTimer(nullptr), isBlinkOn(true), verticalLine(nullptr), horizontalLine(nullptr),pointGroup(nullptr) {
        blinkTimer = new QTimer(this);
        connect(blinkTimer, &QTimer::timeout, this, &XYOverlayGrid::toggleBlink);
        blinkTimer->start(100);
    }
    void updateOverlay(int original_y, int original_z, int imageWidth, int imageHeight) {
        // Clear only the line group, not the entire scene
        if (lineGroup)
        {
            scene->removeItem(lineGroup); // Remove the current group from the scene
            delete lineGroup;             // Delete the old group to prevent memory leaks
            lineGroup = new QGraphicsItemGroup();  // Create a new group for the lines
            scene->addItem(lineGroup);    // Add the new group to the scene
        }
        else
        {
            lineGroup = new QGraphicsItemGroup();
            scene->addItem(lineGroup);  // Add the group to the scene
        }
        double lineSize = 0.003 * std::min(scene->height(), scene->width());
        // Set the pen width to 3 pixels for both lines
        QPen verticalPen(Qt::darkBlue);
        verticalPen.setWidth(lineSize);  // Set width to 3 pixels

        QPen horizontalPen(Qt::magenta);
        horizontalPen.setWidth(lineSize);  // Set width to 3 pixels


        // Add the vertical line
        verticalLine = new QGraphicsLineItem(original_y, 0, original_y, imageHeight);
        verticalLine->setPen(verticalPen);
        lineGroup->addToGroup(verticalLine);

        // Add the horizontal line
        horizontalLine = new QGraphicsLineItem(0, original_z, imageWidth, original_z);
        horizontalLine->setPen(horizontalPen);
        lineGroup->addToGroup(horizontalLine);
    }
    void updatePoints(double pixelX, double pixelY, Qt::GlobalColor vcolor, Qt::GlobalColor hcolor) {
        if (!pointGroup) {
            pointGroup = new QGraphicsItemGroup();
            scene->addItem(pointGroup);  // Add point group to the scene
        }
        else {
            // Clear points group
            for (auto* item : pointGroup->childItems()) {
                pointGroup->removeFromGroup(item);
                delete item;
            }
        }

        QPen pen(Qt::red);
        QBrush brush(Qt::red);
        pen.setWidth(0.1);

        double sceneWidth = scene->width();
        double sceneHeight = scene->height();
        double pointSize = 0.008 * std::min(sceneWidth, sceneHeight);
        // Add points
        QGraphicsEllipseItem* point = new QGraphicsEllipseItem(pixelX- pointSize /2, pixelY- pointSize /2, pointSize, pointSize);
        //QGraphicsEllipseItem* point = new QGraphicsEllipseItem(pixelX- size * resolution/2, pixelY- size * resolution/2, size * resolution, size * resolution);
        point->setPen(pen);
        point->setBrush(brush);
        pointGroup->addToGroup(point);

        //// Lines
        for (auto item : scene->items()) {
            if (item->data(0).toString() == "MarkLine") {
                scene->removeItem(item);
                delete item;
                break;
            }
        }

        auto* MarkLine = new QGraphicsItemGroup();
        MarkLine->setData(0, "MarkLine");
        scene->addItem(MarkLine);

        double artworkWidth, artworkHeight;
        QGraphicsItem* artwork = nullptr;
        for (auto* item : scene->items()) {
            if (item->data(0).toString() == "artwork") {
                artwork = item;
                break;
            }
        }

        if (artwork) {
            QRectF artworkRect = artwork->boundingRect();  
            artworkWidth = artworkRect.width();
            artworkHeight = artworkRect.height();
        }
        double lineSize = 0.001 * std::min(scene->height(), scene->width());

        auto* verticalLine = new QGraphicsLineItem(pixelX, 0, pixelX, artworkHeight);
        QPen gpen(vcolor);
        gpen.setWidth(lineSize);
        gpen.setCosmetic(true);
        verticalLine->setPen(gpen);
        MarkLine->addToGroup(verticalLine);
        if (hcolor != Qt::color0)
        {
            QPen gpen(hcolor);
            gpen.setWidth(lineSize);
            gpen.setCosmetic(true);
            auto* HorizontalLine = new QGraphicsLineItem(0, pixelY, artworkWidth, pixelY);
            HorizontalLine->setPen(gpen);
            MarkLine->addToGroup(HorizontalLine);
        }
    }
    void toggleBlink() {
        static const QVector<QColor> colors = { Qt::red, Qt::darkGreen, Qt::darkMagenta, Qt::yellow, Qt::cyan };
        static int currentColorIndex = 0;

        static int lineGroupBlinkCounter = 0;
        static bool isDashLine = false; 

        isBlinkOn = !isBlinkOn;

        if (pointGroup) {
            for (auto* item : pointGroup->childItems()) {
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

        if (lineGroup) {
            lineGroupBlinkCounter++;
            if (lineGroupBlinkCounter >= 5) {
                lineGroupBlinkCounter = 0;  
                isDashLine = !isDashLine;

                QPen vlinePen = verticalLine->pen();
                QPen hlinePen = horizontalLine->pen();
                if (isDashLine) {
                    vlinePen.setStyle(Qt::DashLine); 
                    hlinePen.setStyle(Qt::DashLine); 
                    vlinePen.setColor(Qt::darkBlue);
                    hlinePen.setColor(Qt::magenta);
                }
                else {
                    vlinePen.setStyle(Qt::SolidLine);
                    hlinePen.setStyle(Qt::SolidLine);
                    vlinePen.setColor(Qt::darkBlue);
                    hlinePen.setColor(Qt::magenta);
                }
                verticalLine->setPen(vlinePen);                
                horizontalLine->setPen(hlinePen);
            }

            verticalLine->show();
            horizontalLine->show();

            scene->update();
        }
    }
    void clearEvery() {
        try {
            delete verticalLine; verticalLine = nullptr;
            delete horizontalLine; horizontalLine = nullptr;
            if (lineGroup) { delete lineGroup; lineGroup = nullptr; }
            if (pointGroup) { delete pointGroup; pointGroup = nullptr; }
        }
        catch (const std::exception& e) { void(0); }
    }
    void ClearLineGroup() {
        try {
            delete verticalLine; verticalLine = nullptr;
            delete horizontalLine; horizontalLine = nullptr;
            if (lineGroup) { delete lineGroup; lineGroup = nullptr; }
        }
        catch (const std::exception& e) { void(0); }
    }
};
#endif // ZOOMABLEGRAPHICSVIEW_H
