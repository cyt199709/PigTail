#include "PigTail.h"
#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    UserLogin* userLogin = new UserLogin;
    userLogin->show();

   /* CCMainWindow* MainWindow = new CCMainWindow;
    MainWindow->show();*/

    return a.exec();
}
