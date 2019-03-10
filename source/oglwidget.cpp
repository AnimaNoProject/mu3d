#include "oglwidget.h"

OGLWidget::OGLWidget(const QString* vsrc
                     ,const  QString* fsrc, QWidget *parent) : QOpenGLWidget(parent)
{
    vshadersrc = vsrc;
    fshadersrc = fsrc;

    // set surface format to request OpenGL 4.5
    QSurfaceFormat surfaceFormat = this->format();
    surfaceFormat.setVersion(4,5);
    this->setFormat(surfaceFormat);
}

OGLWidget::~OGLWidget()
{
    cleanup();
}

void OGLWidget::initializeGL()
{
    // if context gets destroyed, cleanup before initializeGL is called again
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OGLWidget::cleanup);
    initializeOpenGLFunctions();

    glClearColor(1, 1, 1, 1);

    // create new shader program
    shaderprogram = new QOpenGLShaderProgram();
    shaderprogram->addShaderFromSourceFile(QOpenGLShader::Vertex, *vshadersrc);
    shaderprogram->addShaderFromSourceFile(QOpenGLShader::Fragment, *fshadersrc);
    shaderprogram->link();

    shaderprogram->release();
}

void OGLWidget::cleanup()
{
    if(shaderprogram == nullptr)
        return;

    makeCurrent();
    delete shaderprogram;
    shaderprogram = 0;
    doneCurrent();
}

void OGLWidget::paintGL()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void OGLWidget::resizeGL(int w, int h)
{
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{

}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{

}
