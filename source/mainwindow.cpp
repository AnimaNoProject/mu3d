#include "mainwindow.h"

MainWindow::MainWindow(int height, int width, QString title)
{
    // create layout
    QHBoxLayout *layout = new QHBoxLayout();

    _progressBar = new QProgressBar();
    _progressBar->setRange(0, TEMP_MAX);
    _progressBar->setValue(0);
    _progressBar->setFormat("%v out of %m");
    _progressBar->setTextVisible(true);

    struct sysinfo memInfo;
    sysinfo (&memInfo);
    ulong totalPhysMem = memInfo.totalram;
    totalPhysMem *= memInfo.mem_unit;

    _memoryUsage = new QProgressBar();
    _memoryUsage->setRange(0, totalPhysMem / 1000 / 1000 );
    _memoryUsage->setValue(0);
    _memoryUsage->setFormat("%vMB out of %mMB");
    _memoryUsage->setTextVisible(true);

    _timeLabel = new QLabel();
    _timeLabel->setText("Epoch-Time: ");
    // add openglwidegts for rendering
    _modelWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _planarWidget = new OGLPlanarWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _modelWidget->setMinimumSize(height / 2, width /2);
    _planarWidget->setMinimumSize(height / 2, width /2);
    layout->addWidget(_modelWidget);
    layout->addWidget(_planarWidget);

    this->statusBar()->addPermanentWidget(_timeLabel);
    _timeLabel->setFixedWidth(175);
    this->statusBar()->addPermanentWidget(_progressBar, 2);
    this->statusBar()->addPermanentWidget(_memoryUsage);
    _memoryUsage->setFixedWidth(300);
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

    _isUnfolding = false;
    _unfoldTimer = new QTimer(this);
    _memoryUsageTimer = new QTimer(this);
    connect(_unfoldTimer, SIGNAL(timeout()), this, SLOT(unfoldLoop()));
    connect(_memoryUsageTimer, SIGNAL(timeout()), this, SLOT(updateMemoryUsage()));

    _memoryUsageTimer->setInterval(2000);
    _memoryUsageTimer->start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateMemoryUsage()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != nullptr){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    _memoryUsage->setValue(result / 1000);
}

int MainWindow::parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = int(strlen(line));
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
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
    if(_isUnfolding)
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
    _isUnfolding = true;
    _progressBar->setTextVisible(true);
    _progressBar->setValue(0);
    _progressBar->setFormat("%v out of %m");
    _model->_graph.initializeState();
    _model->clearGL();
    _planarWidget->updateGL();
    _modelWidget->updateGL();

    _unfoldTimer->start();
}

void MainWindow::stop()
{
    _unfoldTimer->stop();
    this->setCursor(Qt::ArrowCursor);
    _loadModel->setEnabled(true);
    _unfold->setText("Unfolding");
    _isUnfolding = false;

    std::stringstream ss;
    ss << "Time: " << _unfoldTime << "s";
    _progressBar->setFormat(ss.str().c_str());
}

void MainWindow::unfoldLoop()
{
    clock_gettime(CLOCK_MONOTONIC, &_timeStart);

    bool redraw = _model->recalculate();

    if(redraw)
    {
        _model->clearGL();
        _planarWidget->updateGL();
        _modelWidget->updateGL();
    }

    int iterationsDone = int(TEMP_MAX) - _model->finishedAnnealing();
    _progressBar->setValue(iterationsDone);

    clock_gettime(CLOCK_MONOTONIC, &_timeFinish);
    _epochTime = (_timeFinish.tv_sec - _timeStart.tv_sec) * 1000;
    _epochTime += (_timeFinish.tv_nsec - _timeStart.tv_nsec) / 1000000.0;

    _unfoldTime += _epochTime;

    std::stringstream ss;
    ss << "Epoch-Time: " << std::fixed << std::setprecision(5) << _epochTime << "ms";
    _timeLabel->setText(ss.str().c_str());

    if(iterationsDone >= int(TEMP_MAX))
    {
        stop();
    }
}
