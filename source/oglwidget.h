#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

#include <QMatrix4x4>

#include <QWheelEvent>

#include "model.h"

class Model;

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    OGLWidget(const QString* vshaderFile,const QString* fshaderFile, QWidget *parent = 0);
    ~OGLWidget();
    void importModel(const char* filename);
public slots:
    void cleanup();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    QOpenGLShaderProgram *_program;

    const  QString* _vshaderFile;
    const  QString* _fshaderFile;

    Model* _model;

    QMatrix4x4 _projMatrix;
    QMatrix4x4 _cameraMatrix;
};
