#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>
#include "oglwidget.h"
#include "model.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int height, int width, QString title);
    ~MainWindow();
    void loadModel();
private:
    OGLWidget* _modelWidget;
    OGLWidget* _planarWidget;
};
