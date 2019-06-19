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

    // add openglwidegts for rendering
    _modelWidget = new OGLWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _planarWidget = new OGLPlanarWidget(new QString("./shader/shader.vert"), new QString("./shader/shader.frag"));
    _modelWidget->setMinimumSize(height / 2, width /2);
    _planarWidget->setMinimumSize(height / 2, width /2);
    layout->addWidget(_modelWidget);
    layout->addWidget(_planarWidget);

    this->statusBar()->addPermanentWidget(_progressBar, 2);
    this->statusBar()->addPermanentWidget(_memoryUsage, 2);
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
    memory = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(unfoldLoop()));
    connect(memory, SIGNAL(timeout()), this, SLOT(updateMemoryUsage()));

    memory->setInterval(10000);
    memory->start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateMemoryUsage()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
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
    int i = strlen(line);
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
    _progressBar->setFormat("%v out of %m");
    _model->_graph.initializeState();
    _model->clearGL();
    _planarWidget->updateGL();
    _modelWidget->updateGL();

    clock_gettime(CLOCK_MONOTONIC, &up);
    timer->start();
}

void MainWindow::stop()
{
    timer->stop();
    this->setCursor(Qt::ArrowCursor);
    _loadModel->setEnabled(true);
    _unfold->setText("Unfolding");
    unfolding = false;

    clock_gettime(CLOCK_MONOTONIC, &down);
    elapsed = (down.tv_sec - up.tv_sec);
    elapsed += (down.tv_nsec - up.tv_nsec) / 1000000000.0;

    std::stringstream ss;
    ss << "Time: " << elapsed << "s";
    _progressBar->setFormat(ss.str().c_str());
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

    if(iterationsDone >= int(TEMP_MAX))
    {
        stop();
    }
}
