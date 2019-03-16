#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "CGAL/config.h"
#include "boost/config.hpp"

int main( int argc, char* argv[] )
{
    //std::cout << "CGAL Version" << CGAL_VERSION_NR << std::endl;
    //std::cout << "BGL Verrsion" << BOOST_VERSION << std::endl;

    QApplication app(argc, argv);

    MainWindow window(1000, 1000, "3D Mesh Unfolding");
    window.show();

    return app.exec();
}
