#ifndef QUADTELEMETRY_H
#define QUADTELEMETRY_H

#include <QObject>
#include <QDebug>

#include "HIDAPIWrapper.h"

struct Sensor {
    Sensor() : X(0.0), Y(0.0), Z(0.0) { }

    float X, Y, Z;
};

struct Attitude {
    Attitude() : roll(0.0), pitch(0.0), yaw(0.0) { }

    float roll, pitch, yaw;
};

class QuadTelemetry : public QObject
{
    Q_OBJECT
public:
    explicit QuadTelemetry(QObject *parent = 0);
    ~QuadTelemetry();

signals:
    void attitudeUpdated(const Attitude & a);
    void accelerometerUpdated(const Sensor &a);
    void gyroscopeUpdated(const Sensor &a);
    void magnetometerUpdated(const Sensor &a);

public slots:
    void dataReceived(const unsigned char *receivedData, size_t lenght);
    void toggleLeds();
    void sendDataRequest();

private:
    //----- Métodos utilidade
    inline void parseData(const QByteArray &dataPacket);

private:
    HIDAPIWrapper *m_hidWrapper;
    QTimer *m_checkDataTimer;

    Attitude m_currentAttitude;
    Sensor m_accelerometer;
    Sensor m_gyroscope;
    Sensor m_magnetometer;
};

void QuadTelemetry::parseData(const QByteArray &dataPacket)
{
    if(dataPacket.contains("R-")) {
        if(dataPacket.at(2) == 'S') {
            float accelX, accelY, accelZ;
            float gyroX, gyroY, gyroZ;

            memcpy(&gyroX, dataPacket.constData()+3,  4);
            memcpy(&gyroY, dataPacket.constData()+7,  4);
            memcpy(&gyroZ, dataPacket.constData()+11, 4);

            memcpy(&accelX, dataPacket.constData()+15, 4);
            memcpy(&accelY, dataPacket.constData()+19, 4);
            memcpy(&accelZ, dataPacket.constData()+23, 4);

            m_gyroscope.X = gyroX;
            m_gyroscope.Y = gyroY;
            m_gyroscope.Z = gyroZ;

            m_accelerometer.X = accelX;
            m_accelerometer.Y = accelY;
            m_accelerometer.Z = accelZ;

            emit gyroscopeUpdated(m_gyroscope);
            emit accelerometerUpdated(m_accelerometer);

        }else if(dataPacket.at(2) == 'L') {

            float roll, pitch, yaw;

            float rollControlOutput, pitchControlOutput, yawControlOutput;

            memcpy(&roll,  dataPacket.constData()+3,  4);
            memcpy(&pitch, dataPacket.constData()+7,  4);
            memcpy(&yaw,   dataPacket.constData()+11, 4);

            memcpy(&rollControlOutput,  dataPacket.constData()+15, 4);
            memcpy(&pitchControlOutput, dataPacket.constData()+19, 4);
            memcpy(&yawControlOutput,   dataPacket.constData()+23, 4);

            m_currentAttitude.roll = roll;
            m_currentAttitude.pitch = pitch;
            m_currentAttitude.yaw = yaw;

            emit attitudeUpdated(m_currentAttitude);
        }
    }
}


#endif // QUADTELEMETRY_H
