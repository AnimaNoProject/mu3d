#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QTimer>

#include "oglwidget.h"
#include "oglplanarwidget.h"
#include "model.h"

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

private:
    OGLWidget* _modelWidget;
    OGLPlanarWidget* _planarWidget;
    QAction* _unfold;
    QAction* _loadModel;

    QTimer* timer;
    bool unfolding;
    bool unfoldSuccess;

    void start();
    void stop();
};
