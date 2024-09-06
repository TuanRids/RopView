#include "RopView.h"

#include "MainUI/mainwindow.h"
using namespace std;


int main(int argc, char* argv[]) {
	// if release call FreeConsole();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
#ifdef NDEBUG
    FreeConsole();
#endif
	SetDllDirectoryW(L"libs");
	system("cls");
	nmainUI::UIFrame::getInstance().mainloop(argc, argv);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
}

