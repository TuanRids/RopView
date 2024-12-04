
#include "RopView.h"
#include "MainUI/mainwindow.h"
using namespace std;


int main(int argc, char* argv[]) {
    const wchar_t* pipeName = L"\\\\.\\pipe\\UniqueAppPipe";

    HANDLE hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 0, 0, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY) {
        std::cerr << "Application is already running.\n";
        return 1; 
    }

    std::cout << "Application started.\n";


	nmainUI::UIFrame::getInstance().mainloop(argc, argv);
    CloseHandle(hPipe);
}
