#include "AviewFrame.h"
#include "..\pch.h"


void AviewFrame::update() {

    if (scandat.Amplitudes.empty()) {
        if (sttlogs) { sttlogs->logWarning("No amplitude data available"); }
        return;
    }
    CreateAview();
}


void AviewFrame::CreateAview()
{
    UISetup();
    // ********** PARAMETER VALIDATION **********
    if (scandat.Amplitudes.empty()) {
        if (sttlogs) {
            sttlogs->logWarning("No amplitude data available");
        }
        return;
    }

    // ********** PARAMETER PROCESSING **********
    lineSeries->clear();
    uint64_t zsize = scandat.AmplitudeAxes[0].Quantity;
    uint64_t ysize = scandat.AmplitudeAxes[1].Quantity;
    uint64_t xsize = scandat.AmplitudeAxes[2].Quantity;

    QVector<QPointF> points;
    points.reserve(zsize);

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();

    for (uint64_t z = 0; z < zsize; ++z) {
        uint64_t index = z * (xsize * ysize) + curpt.y * xsize + curpt.x;

        if (index >= scandat.Amplitudes.size()) {
            sttlogs->logWarning("Out of range data: " + std::to_string(index) + " " + std::to_string(scandat.Amplitudes.size()));
            return;
        }

        int16_t samplingAmplitude = std::abs(scandat.Amplitudes[index]);
        double percentAmplitude = samplingAmplitude / (32768 / 100.0);

        minY = std::min(minY, percentAmplitude);
        maxY = std::max(maxY, percentAmplitude);
        points.append(QPointF(percentAmplitude, z));
    }

    lineSeries->replace(points);

    // ********** DISPLAY **********
    axisX->setRange(minY, 100);
    axisY->setRange(0, zsize);
    axisY->setReverse(true);

    int viewWidth = graphicsView->width();
    int viewHeight = graphicsView->height();
    chartView->setFixedSize(viewWidth, viewHeight);
    chartView->setGeometry(-10, 10, viewWidth, viewHeight);

    graphicsView->update();

    if (!isPanning) {
        sttlogs->logInfo("Coord x: " + std::to_string(curpt.x) + " - Coord y: " + std::to_string(curpt.y) + " Coord z: " + std::to_string(curpt.z) + ".");
    }

}


