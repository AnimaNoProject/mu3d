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
#include <QLabel>
#include <QCheckBox>

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
    QLabel* _timeLabel;
    QAction* _bruteForce;


    struct timespec _timeStart, _timeFinish;
    double _epochTime;
    double _unfoldTime;

    Model* _model;

    QTimer* _unfoldTimer;
    QTimer* _memoryUsageTimer;

    bool _isUnfolding;
    bool _unfoldSuccess;

    int iterations;

    void anneal();

    void bruteforce();

    void start();
    void stop();
    int parseLine(char* line);
};
