#ifndef PCH_H
#define PCH_H

// ======== Standard C++ Headers ============
#include <ctime>
#include <iomanip>
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
#include <deque>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
// ======== Windows-Specific Headers ============
#include <windows.h>
// ======== Boost Library ============
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/di.hpp"
#include <boost/lockfree/queue.hpp>



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
#include <QtCharts/qsplineseries.h>
#include <QtCharts/QValueAxis>

// ======== Project-Specific Headers ============
#include <instrumentation/Instrumentation.h>
#include <Storage/Storage.h>
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

// ======== Olympus ============
#include <Storage/Storage.h>


// ======== Logs ============
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h> 
#endif // PCH_H


