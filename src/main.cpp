#include "gui/mainwindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    // QMainWindow window;
    // window.resize(800, 600);
    // window.show();

    MainWindow window;

    window.show();

    return a.exec();
}