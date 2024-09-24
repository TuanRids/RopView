#include "IUIWindow.h"

// Define the static variables outside of any class or function
unsigned int UIWindow::resolution = 12;
bool UIWindow::isCscanLayer = false;
nmainUI::statuslogs* UIWindow::sttlogs = nullptr;
std::shared_ptr<AscanProcessor> UIWindow::processor = nullptr;
std::unique_ptr<QSettings> UIWindow::settings = nullptr;