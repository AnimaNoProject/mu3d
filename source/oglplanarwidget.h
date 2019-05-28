#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

#include <QMatrix4x4>
#include <QPoint>

#include <QWheelEvent>

#include "camera.h"
#include "model.h"
#include "oglwidget.h"

class Model;

class OGLPlanarWidget : public OGLWidget
{
    Q_OBJECT

public:
    OGLPlanarWidget(const QString* vshaderFile,const QString* fshaderFile, QWidget *parent = nullptr);
    virtual ~OGLPlanarWidget() override;
    virtual void updateGL() override;
    virtual void setModel(Model* model) override;

public slots:
    void cleanup();
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};
