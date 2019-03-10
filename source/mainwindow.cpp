#include "mainwindow.h"

MainWindow::MainWindow(int height, int width, QString title)
{
    // create layout
    QHBoxLayout *layout = new QHBoxLayout();

    // add openglwidegts for rendering
    model = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    planar = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    model->setFixedSize(height / 2, width / 2);
    planar->setFixedSize(height / 2, width / 2);
    layout->addWidget(model);
    layout->addWidget(planar);

    // Add action to load a model
    QAction *loadFile = this->menuBar()->addMenu("File")->addAction("Load Model");
    QObject::connect(loadFile, &QAction::triggered, this, &MainWindow::loadModel);

    // setup window
    this->setCentralWidget(new QWidget);
    this->centralWidget()->setLayout(layout);
    this->setWindowTitle(title);
    this->setWindowModality(Qt::ApplicationModal);
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadModel()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose 3D Model", QDir::currentPath(), "OFF Files (*.off)");
    if(filename.isNull())
        return;

    // set model
    Model* model = new Model(filename.toLocal8Bit().data());
}
