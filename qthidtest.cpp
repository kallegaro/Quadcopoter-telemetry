#include "qthidtest.h"
#include "ui_qthidtest.h"

void QtHIDTest::configureAttitudePlot()
{
    ui->attitudePlotWidget->addGraph();
    ui->attitudePlotWidget->addGraph();
    ui->attitudePlotWidget->addGraph();

    ui->attitudePlotWidget->graph(0)->setPen(QPen(QColor("red")));
    ui->attitudePlotWidget->graph(1)->setPen(QPen(QColor("green")));
    ui->attitudePlotWidget->graph(2)->setPen(QPen(QColor("blue")));

    ui->attitudePlotWidget->graph(0)->setName("Pitch");
    ui->attitudePlotWidget->graph(1)->setName("Roll");
    ui->attitudePlotWidget->graph(2)->setName("Yaw");
    ui->attitudePlotWidget->legend->setVisible(true);

    ui->attitudePlotWidget->xAxis->setLabel("x");
    ui->attitudePlotWidget->yAxis->setLabel("y");

    ui->attitudePlotWidget->xAxis->setRange(0, 0);
    ui->attitudePlotWidget->yAxis->setRange(-45, 45);
    ui->attitudePlotWidget->replot();
}

void QtHIDTest::configureAccelerometerPlot()
{
    ui->accelerometerPlotWidget->addGraph();
    ui->accelerometerPlotWidget->addGraph();
    ui->accelerometerPlotWidget->addGraph();

    ui->accelerometerPlotWidget->graph(0)->setPen(QPen(QColor("red")));
    ui->accelerometerPlotWidget->graph(1)->setPen(QPen(QColor("green")));
    ui->accelerometerPlotWidget->graph(2)->setPen(QPen(QColor("blue")));

    ui->accelerometerPlotWidget->graph(0)->setName("Accel. X");
    ui->accelerometerPlotWidget->graph(1)->setName("Accel. Y");
    ui->accelerometerPlotWidget->graph(2)->setName("Accel. Z");
    ui->accelerometerPlotWidget->legend->setVisible(true);

    ui->accelerometerPlotWidget->xAxis->setLabel("x");
    ui->accelerometerPlotWidget->yAxis->setLabel("y");

    ui->accelerometerPlotWidget->xAxis->setRange(0, 0);
    ui->accelerometerPlotWidget->yAxis->setRange(-1.5, 1.5);
    ui->accelerometerPlotWidget->replot();
}

void QtHIDTest::configureMagnetometerPlot()
{
    ui->magnetometerPlotWidget->addGraph();
    ui->magnetometerPlotWidget->addGraph();
    ui->magnetometerPlotWidget->addGraph();

    ui->magnetometerPlotWidget->graph(0)->setPen(QPen(QColor("red")));
    ui->magnetometerPlotWidget->graph(1)->setPen(QPen(QColor("green")));
    ui->magnetometerPlotWidget->graph(2)->setPen(QPen(QColor("blue")));

    ui->magnetometerPlotWidget->graph(0)->setName("Mag. X");
    ui->magnetometerPlotWidget->graph(1)->setName("Mag. Y");
    ui->magnetometerPlotWidget->graph(2)->setName("Mag. Z");
    ui->magnetometerPlotWidget->legend->setVisible(true);

    ui->magnetometerPlotWidget->xAxis->setLabel("x");
    ui->magnetometerPlotWidget->yAxis->setLabel("y");

    ui->magnetometerPlotWidget->xAxis->setRange(0, 0);
    ui->magnetometerPlotWidget->yAxis->setRange(-1.5, 1.5);
    ui->magnetometerPlotWidget->replot();
}

QtHIDTest::QtHIDTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtHIDTest),
    m_telemetry(new QuadTelemetry(this)),
    m_receptionCounter(0),
    m_startTimestamp(QDateTime::currentMSecsSinceEpoch()),
    m_msSinceStart(0),
    m_logFile(NULL)
{
    ui->setupUi(this);

    configureAttitudePlot();
    configureAccelerometerPlot();
    configureMagnetometerPlot();

    connect(m_telemetry, &QuadTelemetry::attitudeUpdated, this, &QtHIDTest::plotAttitude);
    connect(m_telemetry, &QuadTelemetry::accelerometerUpdated, this, &QtHIDTest::plotAccelerometer);
    connect(m_telemetry, &QuadTelemetry::gyroscopeUpdated, this, &QtHIDTest::plotMagnetometer);

    connect(ui->menuFile->actions().at(0), &QAction::triggered, this, &QtHIDTest::saveTriggered);
}


QtHIDTest::~QtHIDTest()
{
    delete m_telemetry;
    delete ui;
}

void QtHIDTest::plotMagnetometer(const Sensor &a)
{
    m_msSinceStart = QDateTime::currentMSecsSinceEpoch() - m_startTimestamp;

    ui->magnetometerPlotWidget->graph(0)->addData(m_msSinceStart, a.X);
    ui->magnetometerPlotWidget->graph(1)->addData(m_msSinceStart, a.Y);
    ui->magnetometerPlotWidget->graph(2)->addData(m_msSinceStart, a.Z);

    ui->magXLineEdit->setText(QString::number(a.X, 'f', 3));
    ui->magYLineEdit->setText(QString::number(a.Y, 'f', 3));
    ui->magZLineEdit->setText(QString::number(a.Z, 'f', 3));

    m_receptionCounter++;

    if(m_msSinceStart < 10000) {
        ui->magnetometerPlotWidget->xAxis->setRange(0, m_msSinceStart);
    }else {
        ui->magnetometerPlotWidget->xAxis->setRange(m_msSinceStart-10000, m_msSinceStart);
    }

    ui->magnetometerPlotWidget->replot();
}

void QtHIDTest::saveTriggered(bool newValue)
{
    qDebug() << "Novo estado: " << newValue;

    if(!m_logFile) {
        if(newValue) {
            QString fileName = QFileDialog::getSaveFileName(this, "Open log file");
            qDebug() << "Nome do arquivo: " << fileName;
            if(!fileName.isEmpty()) {

                m_logFile = new QFile(fileName, this);
                if(m_logFile->open(QFile::Append | QFile::WriteOnly)) {
                    qDebug() << "Arquivo aberto";
                }
            }
        }else {
            ui->menuFile->actions().at(0)->setChecked(false);
        }
    }else {
        qDebug () << "Arquivo fechado";
        m_logFile->flush();
        m_logFile->close();
        delete m_logFile;
        m_logFile = NULL;
    }
}

void QtHIDTest::plotAttitude(const Attitude &a)
{
    if(m_logFile) {
        m_logFile->write("Testando escrita \n");
        m_logFile->flush();
    }

    m_msSinceStart = QDateTime::currentMSecsSinceEpoch() - m_startTimestamp;

    ui->attitudePlotWidget->graph(0)->addData(m_msSinceStart, a.pitch);
    ui->attitudePlotWidget->graph(1)->addData(m_msSinceStart, a.roll);
    ui->attitudePlotWidget->graph(2)->addData(m_msSinceStart, a.yaw);

    ui->pitchLineEdit->setText(QString::number(a.pitch, 'f', 3));
    ui->rollLineEdit->setText(QString::number(a.roll, 'f', 3));
    ui->yawLineEdit->setText(QString::number(a.yaw, 'f', 3));

    m_receptionCounter++;

    if(m_msSinceStart < 10000) {
        ui->attitudePlotWidget->xAxis->setRange(0, m_msSinceStart);
    }else {
        ui->attitudePlotWidget->xAxis->setRange(m_msSinceStart-10000, m_msSinceStart);
    }

    ui->attitudePlotWidget->replot();
}

void QtHIDTest::plotAccelerometer(const Sensor &a)
{
    m_msSinceStart = QDateTime::currentMSecsSinceEpoch() - m_startTimestamp;

    ui->accelerometerPlotWidget->graph(0)->addData(m_msSinceStart, a.X);
    ui->accelerometerPlotWidget->graph(1)->addData(m_msSinceStart, a.Y);
    ui->accelerometerPlotWidget->graph(2)->addData(m_msSinceStart, a.Z);

    ui->accelXLineEdit->setText(QString::number(a.X, 'f', 3));
    ui->accelYLineEdit->setText(QString::number(a.Y, 'f', 3));
    ui->accelZLineEdit->setText(QString::number(a.Z, 'f', 3));

    m_receptionCounter++;

    if(m_msSinceStart < 10000) {
        ui->accelerometerPlotWidget->xAxis->setRange(0, m_msSinceStart);
    }else {
        ui->accelerometerPlotWidget->xAxis->setRange(m_msSinceStart-10000, m_msSinceStart);
    }

    ui->accelerometerPlotWidget->replot();
}

void QtHIDTest::plotGyroscope(const Sensor &a)
{

}
