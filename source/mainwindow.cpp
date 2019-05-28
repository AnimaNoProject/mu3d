#include "mainwindow.h"

MainWindow::MainWindow(int height, int width, QString title)
{
    // create layout
    QHBoxLayout *layout = new QHBoxLayout();

    _progressBar = new QProgressBar();
    _progressBar->setRange(0, TEMP_MAX);
    _progressBar->setValue(0);
    _progressBar->setTextVisible(false);

    // add openglwidegts for rendering
    _modelWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _planarWidget = new OGLPlanarWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _modelWidget->setMinimumSize(height / 2, width /2);
    _planarWidget->setMinimumSize(height / 2, width /2);
    layout->addWidget(_modelWidget);
    layout->addWidget(_planarWidget);

    this->statusBar()->addPermanentWidget(_progressBar, 2);
    this->statusBar()->show();

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

    unfolding = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(unfoldLoop()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadModel()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose 3D Model", QDir::currentPath(), "OFF Files (*.off)");
    if(filename.isNull())
        return;

    _model = new Model(filename.toLocal8Bit().data());
    _unfold->setDisabled(false);

    _modelWidget->setModel(_model);
}

void MainWindow::unfoldModel()
{
    if(unfolding)
    {
        stop();
    }
    else
    {
        start();
    }
}

void MainWindow::start()
{
    this->setCursor(Qt::WaitCursor);
    _loadModel->setEnabled(false);
    _planarWidget->setModel(_model);
    _unfold->setText("Stop Unfolding");
    unfolding = true;
    _progressBar->setTextVisible(true);
    _progressBar->setValue(0);
    _model->_graph.initializeState();
    _model->clearGL();
    _planarWidget->updateGL();
    _modelWidget->updateGL();

    timer->start(1);
}

void MainWindow::stop()
{
    timer->stop();
    this->setCursor(Qt::ArrowCursor);
    _loadModel->setEnabled(true);
    _unfold->setText("Unfolding");
    unfolding = false;
    _progressBar->setFormat("Finished.");
}

void MainWindow::unfoldLoop()
{
    bool redraw = _model->recalculate();

    if(redraw)
    {
        _model->clearGL();
        _planarWidget->updateGL();
        _modelWidget->updateGL();
    }

    int iterationsDone = int(TEMP_MAX) - _model->finishedAnnealing();

    _progressBar->setValue(iterationsDone);
    std::stringstream ss;
    ss << "Iteration " << iterationsDone << "/" << TEMP_MAX;
    _progressBar->setFormat(ss.str().c_str());

    if(iterationsDone >= int(TEMP_MAX))
    {
        stop();
    }
}
