#include "PigTail.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PigTail w;
    w.show();
    return a.exec();
}
