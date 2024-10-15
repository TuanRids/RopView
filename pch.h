#ifndef PCH_H
#define PCH_H

// ======== Standard C++ Headers ============
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <chrono>

// ======== Windows-Specific Headers ============
#include <windows.h>

// ======== JSON Parsing Library ============
#include "nlohmann/json.hpp"

// ======== QT UI ============
#include <QApplication>
#include <QTextEdit>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QFrame>
#include <QSettings>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QTime>
#include <QTabWidget>
#include <QMessageBox>
#include <QFileDialog>

// ======== Additional QT UI Components ============
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QDockWidget>
#include <QSize>
#include <QStyleFactory>
#include <QGraphicsTextItem>
#include <QSpinBox>
#include <QToolTip>
#include <QMenu>
#include <QGroupBox>
#include <qcustomplot.h>
#include <QMenuBar>
#include <QMenu>

// ======== Qt Charts ============
#include <QtCharts/qchart.h>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

// ======== Project-Specific Headers ============
#include <instrumentation/Instrumentation.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ======== QTVulkan ============
#include <QVulkanWindow>
#include <QVulkanFunctions>
#include <QVulkanFunctions>
#include <QApplication>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLCDNumber>

// ======== OpenCV CUDA ============
#include <opencv2/opencv.hpp>
#endif // PCH_H
