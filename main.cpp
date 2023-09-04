#include "BatchItImage.h"

// The following line is necessary for the GetConsoleWindow() function to work!
// It basically says that you are running this program on Windows 2000 or higher
#define _WIN32_WINNT 0x0500
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
#ifdef _DEBUG
    // Show Console Window
    HWND console = GetConsoleWindow();
    MoveWindow(console, 0, 0, 640, 1090, TRUE);
#endif
    QApplication a(argc, argv);
    BatchItImage w;
    w.show();
    return a.exec();
}
