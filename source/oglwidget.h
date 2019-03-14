#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QOpenGLVertexArrayObject>
#include "model.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    OGLWidget(const QString* vshadersrc,const QString* fshadersrc, QWidget *parent = 0);
    ~OGLWidget();
    void importModel(Model *model);
public slots:
    void cleanup();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    QOpenGLShaderProgram *_program;

    const  QString* _vshadersrc;
    const  QString* _fshadersrc;

    Model* _model;

    QMatrix4x4 _projMatrix;
    QMatrix4x4 _cameraMatrix;
    QMatrix4x4 _modelMatrix;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;

    void createGLModelContext();
};
