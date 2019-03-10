#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include "model.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    OGLWidget(const QString* vshadersrc,const QString* fshadersrc, QWidget *parent = 0);
    ~OGLWidget();
public slots:
    void cleanup();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QOpenGLShaderProgram *shaderprogram;
    const  QString* vshadersrc;
    const  QString* fshadersrc;
    Model* model;
};
