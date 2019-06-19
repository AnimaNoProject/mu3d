#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QTimer>
#include <QProgressBar>
#include <QString>
#include <QStatusBar>

#include "oglwidget.h"
#include "oglplanarwidget.h"
#include "model.h"
#include "graph.h"

#include <thread>

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int height, int width, QString title);
    ~MainWindow();
    void loadModel();
    void unfoldModel();
public slots:
    void unfoldLoop();
    void updateMemoryUsage();
private:
    OGLWidget* _modelWidget;
    OGLPlanarWidget* _planarWidget;
    QAction* _unfold;
    QAction* _loadModel;
    QProgressBar * _progressBar;
    QProgressBar * _memoryUsage;

    struct timespec up, down;
    double elapsed;

    Model* _model;

    QTimer* timer;
    QTimer* memory;

    bool unfolding;
    bool unfoldSuccess;


    void start();
    void stop();


    int parseLine(char* line);
};
