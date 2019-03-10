#include <iostream>
#include <QApplication>
#include "mainwindow.h"

int main( int argc, char* argv[] )
{
    QApplication app(argc, argv);

    MainWindow window(1000, 1000, "3D Mesh Unfolding");

    window.show();

    return app.exec();
}
