#include "oglwidget.h"

OGLWidget::OGLWidget(const QString* vshaderFile,const QString* fshaderFile, QWidget *parent) : QOpenGLWidget(parent)
{
    _vshaderFile = vshaderFile;
    _fshaderFile = fshaderFile;

    // set surface format to request OpenGL 4.5
    QSurfaceFormat surfaceFormat = this->format();
    surfaceFormat.setVersion(4,5);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(surfaceFormat);

    this->setFocusPolicy(Qt::ClickFocus); // focus for key events can be gained by clicking or tabbing
}

void OGLWidget::importModel(const char* filename)
{
    // makeCurrent is important, if this is not called, the model cannot
    // create the VBO/IBO/VAO in the context of this widget
    makeCurrent();
    _model = new Model(filename, _program); // create the new model
    doneCurrent();
    update(); // update calls paintGL to renew the rendering
}

OGLWidget::~OGLWidget()
{
    cleanup();
}

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // if context gets destroyed, cleanup before initializeGL is called again
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OGLWidget::cleanup);
    glClearColor(0, 0, 0, 1);

    // create new shader program
    _program = new QOpenGLShaderProgram();
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, *_vshaderFile);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, *_fshaderFile);
    _program->bindAttributeLocation("position", 0);
    _program->link();

    _model = new Model(_program); // use default model until something is imported

    // move back, camera will be constant
    _cameraMatrix.setToIdentity();
    _cameraMatrix.translate(0, 0, -20);
}

void OGLWidget::cleanup()
{
    // if no program was initialized return
    if(_program == nullptr)
        return;

    // delete the program and buffer
    makeCurrent();
    delete _program;
    _program = 0;
    doneCurrent();
}

void OGLWidget::paintGL()
{
    // clear widget and enable depth testing + face culling
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // bind shaderprogram and set variables
    _program->bind();
    _program->setUniformValue(_program->uniformLocation("viewProjMatrix"), _projMatrix * _cameraMatrix);
    _model->draw();

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
    }
    update();
}

void OGLWidget::wheelEvent(QWheelEvent *event)
{

    float steps = (event->angleDelta().y() / 8) / 15;
    _cameraMatrix.translate(0, 0, steps * 0.5f);
    update();
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{

}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{

}
