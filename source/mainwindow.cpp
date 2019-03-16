#include "mainwindow.h"

MainWindow::MainWindow(int height, int width, QString title)
{
    // create layout
    QHBoxLayout *layout = new QHBoxLayout();

    // add openglwidegts for rendering
    _modelWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _planarWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _modelWidget->setMinimumSize(height / 2, width /2);
    _planarWidget->setMinimumSize(height / 2, width /2);
    layout->addWidget(_modelWidget);
    layout->addWidget(_planarWidget);

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

    _modelWidget->importModel(filename.toLocal8Bit().data());
}
