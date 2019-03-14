#include "oglwidget.h"

OGLWidget::OGLWidget(const QString* vshadersrc,const QString* fshadersrc, QWidget *parent) : QOpenGLWidget(parent)
{
    _vshadersrc = vshadersrc;
    _fshadersrc = fshadersrc;
    // set surface format to request OpenGL 4.5
    QSurfaceFormat surfaceFormat = this->format();
    surfaceFormat.setVersion(4,5);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(surfaceFormat);

    _model = new Model(); // use default model until something is imported
}

void OGLWidget::importModel(Model *model)
{
    _model = model;
    makeCurrent();
    _ibo.destroy();
    _vbo.destroy();
    _vao.destroy();

    createGLModelContext();
    doneCurrent();
    update();
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
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, *_vshadersrc);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, *_fshadersrc);
    _program->bindAttributeLocation("position", 0);
    _program->link();

    createGLModelContext();

    // move back
    _cameraMatrix.setToIdentity();
    _cameraMatrix.translate(0, 0, -15);

    _modelMatrix.setToIdentity();
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
    _ibo.destroy();
    _vbo.destroy();
    _vao.destroy();
    doneCurrent();
}

void OGLWidget::createGLModelContext()
{
    // delcare Vertex and Index buffer
    _vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    _program->bind();

    // create and bind VAO
    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    // create VBO and allocate buffer
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_model->getVertices().data(), _model->getVertices().size() * sizeof(QVector3D));
    _vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    _vbo.release();

    // create IBO and allocate buffer
    _ibo.create();
    _ibo.bind();
    _ibo.allocate(_model->getIndices().data(), _model->getIndices().size() * sizeof(GLushort));
    _ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    vaoBinder.release();
    _ibo.release();

    _program->release();
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    _program->bind();

    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _program->setUniformValue(_program->uniformLocation("modelMatrix"), _model->getModelMatrix());
    _program->setUniformValue(_program->uniformLocation("viewProjMatrix"), _projMatrix * _cameraMatrix);
    glDrawElements(GL_TRIANGLES, _model->getIndices().size(), GL_UNSIGNED_SHORT, 0);

    _program->release();
}

void OGLWidget::resizeGL(int w, int h)
{
    _projMatrix.setToIdentity();
    _projMatrix.perspective(45.f, float(w) / h, 0.01f, 1000.0f);
}

void OGLWidget::mousePressEvent(QMouseEvent *event){}
void OGLWidget::mouseMoveEvent(QMouseEvent *event){}
