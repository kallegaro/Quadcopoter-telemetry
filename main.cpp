#include "qthidtest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtHIDTest w;
    w.show();

    return a.exec();
}
