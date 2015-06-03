#include "QuadTelemetry.h"

QuadTelemetry::QuadTelemetry(QObject *parent) : QObject(parent),
    m_hidWrapper(new HIDAPIWrapper(0x2004, 0x0001, 64, 64, this)),
    m_checkDataTimer(new QTimer(this)),
    m_currentAttitude(),
    m_accelerometer(),
    m_gyroscope(),
    m_magnetometer()
{
    //----- Configurações do timer de checagem de dados
    m_checkDataTimer->setInterval(1);
    m_checkDataTimer->start();

    //----- Connects utilizados pelo software
    connect(m_checkDataTimer, &QTimer::timeout, this, &QuadTelemetry::sendDataRequest);
    connect(m_hidWrapper, &HIDAPIWrapper::dataReceived, this, &QuadTelemetry::dataReceived);

    m_hidWrapper->configurePollingTimer(1);
}

QuadTelemetry::~QuadTelemetry()
{
    delete m_hidWrapper;
    delete m_checkDataTimer;
}


void QuadTelemetry::toggleLeds()
{
    QByteArray teste;
    teste.append('S');
    m_hidWrapper->sendData(teste);
}


void QuadTelemetry::sendDataRequest()
{
    QByteArray teste;
    teste.append('N');
    m_hidWrapper->sendData(teste);
}



void QuadTelemetry::dataReceived(const QByteArray & dataPacket)
{
    parseData(dataPacket);
}
