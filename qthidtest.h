#ifndef QTHIDTEST_H
#define QTHIDTEST_H

#include <QFile>
#include <QMainWindow>
#include <QFileDialog>
#include <QTimer>
#include <QTime>

#include "QuadTelemetry.h"
#include "HIDAPIWrapper.h"
#include "qcustomplot.h"

namespace Ui {
class QtHIDTest;
}

class QtHIDTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtHIDTest(QWidget *parent = 0);
    ~QtHIDTest();

    void configureAttitudePlot();
    void configureAccelerometerPlot();
    void configureMagnetometerPlot();
public slots:
    void plotAttitude(const Attitude &a);
    void plotAccelerometer(const Sensor &a);
    void plotGyroscope(const Sensor &a);
    void plotMagnetometer(const Sensor &a);

    void saveTriggered(bool newValue);

private:
    Ui::QtHIDTest *ui;

    QuadTelemetry *m_telemetry;

    quint64 m_receptionCounter;
    quint64 m_startTimestamp;
    quint64 m_msSinceStart;

    QFile *m_logFile;
};


#endif // QTHIDTEST_H
