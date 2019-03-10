#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QHBoxLayout>
#include "oglwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int height, int width, QString title);
    ~MainWindow();
private:
    OGLWidget *model;
    OGLWidget *planar;

    void loadModel();
};
