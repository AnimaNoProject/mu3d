#include "mainwindow.h"

MainWindow::MainWindow(int height, int width, QString title)
{
    // create layout
    QHBoxLayout *layout = new QHBoxLayout();

    // add openglwidegts for rendering
    _modelWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _planarWidget = new OGLPlanarWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _modelWidget->setMinimumSize(height / 2, width /2);
    _planarWidget->setMinimumSize(height / 2, width /2);
    layout->addWidget(_modelWidget);
    layout->addWidget(_planarWidget);

    // Add action to load a model
    _loadModel = this->menuBar()->addAction("Load Model");
    _unfold = this->menuBar()->addAction("Unfold");

    _unfold->setDisabled(true);
    QObject::connect(_loadModel, &QAction::triggered, this, &MainWindow::loadModel);
    QObject::connect(_unfold, &QAction::triggered, this, &MainWindow::unfoldModel);

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
    _unfold->setDisabled(false);
}

void MainWindow::unfoldModel()
{
    _planarWidget->add(_modelWidget->_model);
    _unfold->setDisabled(true);

    //std::thread t1(&OGLPlanarWidget::unfold, _planarWidget);
    _planarWidget->unfold();
}
