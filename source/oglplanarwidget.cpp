#include "oglplanarwidget.h"

OGLPlanarWidget::OGLPlanarWidget(const QString* vshaderFile,const QString* fshaderFile, QWidget *parent) : OGLWidget(vshaderFile, fshaderFile, parent)
{
}

OGLPlanarWidget::~OGLPlanarWidget()
{
    cleanup();
}

void OGLPlanarWidget::add(Model* model)
{
    _model = model;
}

void OGLPlanarWidget::unfold()
{
    makeCurrent();
    _model->unfold();
    doneCurrent();
    _initialized = true;
    update();
}

void OGLPlanarWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // if context gets destroyed, cleanup before initializeGL is called again
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OGLWidget::cleanup);
    glClearColor(1.0, 1.0, 1.0, 1);

    // create new shader program
    _program = new QOpenGLShaderProgram();
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, *_vshaderFile);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, *_fshaderFile);
    _program->bindAttributeLocation("position", 0);
    _program->link();

    _camera = new Camera();
}

void OGLPlanarWidget::cleanup()
{
    // if no program was initialized return
    if(_program == nullptr)
        return;

    // delete the program and buffer
    makeCurrent();
    delete _program;
    _program = nullptr;
    doneCurrent();
}

void OGLPlanarWidget::paintGL()
{
    // clear widget and enable depth testing + face culling
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // bind shaderprogram and set variables
    _program->bind();
    _program->setUniformValue(_program->uniformLocation("viewProjMatrix"), _projMatrix * _camera->getMatrix());

    if(_initialized)
    {
        _model->drawPlanarPatch(_program);
    }

    _program->release();
}

void OGLPlanarWidget::resizeGL(int w, int h)
{    update();
    // on resize the projection matrix needs to be updated
    _projMatrix.setToIdentity();
    _projMatrix.perspective(45.f, float(w) / h, 0.01f, 1000.0f);
}

void OGLPlanarWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!_initialized)
        return;

    float dx = (event->x() - _lastPos.x()) * 0.045f;
    float dy = (event->y() - _lastPos.y()) * 0.045f;

    if(event->buttons() == Qt::LeftButton)
    {
        //_camera->translate(-dx, -dy);
        _model->_modelMatrixPlanar.translate(dx, -dy, 0);
        update();
    }

    _lastPos = event->pos();
}

void OGLPlanarWidget::keyPressEvent(QKeyEvent *event)
{
    if(!_initialized)
        return;

    switch(event->key())
    {
        case Qt::Key_F1:
            break;
        case Qt::Key_F2:
            break;
    }
}

