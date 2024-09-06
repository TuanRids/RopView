// Standard C++ headers
#include <iostream>      // Standard input/output streams
#include <fstream>       // File input/output streams
#include <memory>        // Smart pointers (unique_ptr, shared_ptr)
#include <string>        // String operations
#include <vector>        // Vector container
#include <algorithm>     // Common algorithms (sort, find, etc.)
#include <cassert>       // Assert macro for debugging
#include <cstdio>        // C-style I/O functions
#include <cstdlib>       // C standard library functions (exit, malloc, etc.)
#include <chrono>        // Time-related utilities

// Windows-specific headers
#include <windows.h>     // Windows API (for system-level operations)

// JSON Parsing Library (nlohmann)
#include "nlohmann/json.hpp"  // For parsing JSON data
#ifndef PCH_H
#define PCH_H
// Qt Framework headers (for building UI components)
#include <QApplication>      // Application management
#include <QTextEdit>         // Multi-line text editor widget
#include <QWidget>           // Base class for all UI objects
#include <QPushButton>       // Button widget
#include <QMainWindow>       // Main window framework
#include <QVBoxLayout>       // Vertical box layout manager
#include <QLabel>            // Label widget for displaying text
#include <QSplitter>         // Widget for splitting areas horizontally/vertically
#include <QFrame>            // Frame widget for grouping and styling widgets
#include <QSettings>         // Settings handler (for saving/restoring application state)
#include <QObject>           // Base class for all Qt objects, provides signal/slot mechanism
#include <QString>           // Qt string class
#include <QFile>             // File I/O in Qt
#include <QTextStream>       // Stream for reading/writing text files
#include <QMutex>            // Mutex for thread synchronization
#include <QTime>             // Time representation in Qt

// Additional Qt UI components
#include <QComboBox>         // Dropdown list widget
#include <QSlider>           // Slider widget for selecting a range
#include <QLineEdit>         // Single-line text editor widget
#include <QCheckBox>         // Checkable box widget
#include <QRadioButton>      // Radio button widget
#include <QDockWidget>		 // Dock widget
#include <QSize>			 // Qsize
#include <QStyleFactory>	 // QStyleFactory

// Project-specific headers
#include <instrumentation/Instrumentation.h>  // Instrumentation for performance tracking
#endif // PCH_H
