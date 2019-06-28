#include "oglwidget.h"

OGLWidget::OGLWidget(const QString* vshaderFile,const QString* fshaderFile, QWidget *parent) : QOpenGLWidget(parent)
{
    _vshaderFile = vshaderFile;
    _fshaderFile = fshaderFile;

    // set surface format to request OpenGL 4.5
    QSurfaceFormat surfaceFormat = this->format();
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSamples(4);
    surfaceFormat.setVersion(4,5);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    this->setFormat(surfaceFormat);
    this->create();
    this->setFocusPolicy(Qt::ClickFocus); // focus for key events can be gained by clicking or tabbing
    _initialized = false;
    boostZoom = false;
}

void OGLWidget::setModel(Model* model)
{
    _model = model;

    // makeCurrent is important, if this is not called, the model cannot
    makeCurrent();
    _model->createGLModelContext(_program);
    _camera->reset();
    doneCurrent();
    _initialized = true;
    update(); // update calls paintGL to renew the rendering
}

OGLWidget::~OGLWidget()
{
    cleanup();
}

void OGLWidget::updateGL()
{
    makeCurrent();
    _model->load3DGL(_program);
    doneCurrent();
    update();
}

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // if context gets destroyed, cleanup before initializeGL is called again
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OGLWidget::cleanup);
    glClearColor(0.5, 0.5, 0.5, 1);

    // create new shader program
    _program = new QOpenGLShaderProgram();
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, *_vshaderFile);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, *_fshaderFile);
    _program->bindAttributeLocation("position", 0);
    _program->link();

    _camera = new Camera();
}

void OGLWidget::cleanup()
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

void OGLWidget::paintGL()
{
    // clear widget and enable depth testing + face culling
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    // bind shaderprogram and set variables
    _program->bind();
    _program->setUniformValue(_program->uniformLocation("viewProjMatrix"), _projMatrix * _camera->getMatrix());

    if(_initialized)
    {
        _model->draw(_program);
    }

    _program->release();
}

void OGLWidget::resizeGL(int w, int h)
{
    // on resize the projection matrix needs to be updated
    _projMatrix.setToIdentity();
    _projMatrix.perspective(45.f, float(w) / h, 0.01f, 1000.0f);
}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        // Key_F1 -> switch between WireFrame and filled mode
        case Qt::Key_F1:
            _model->switchRenderMode();
            break;
        case Qt::Key_F2:
            _model->showGluetags();
            break;
        case Qt::Key_F3:
            boostZoom = !boostZoom;
            break;
    }
    update();
}

void OGLWidget::wheelEvent(QWheelEvent *event)
{
    float steps = (event->angleDelta().y() / 8) / 15;

    _camera->zoom(steps * 0.5f, boostZoom);
    update();
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    _lastPos = event->pos();
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = event->x() - _lastPos.x();
    float dy = event->y() - _lastPos.y();

    if(event->buttons() == Qt::LeftButton)
    {
        _camera->rotate(dx, dy);
        update();
    }

    _lastPos = event->pos();
}
