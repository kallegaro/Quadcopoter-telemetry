#ifndef HIDAPIWRAPPER_H
#define HIDAPIWRAPPER_H

#include <QDebug>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTimer>

#include <iostream>

#include <libusb.h>
//#include <hidapi.h>

class HIDAPIWrapper : public QObject
{
    Q_OBJECT
public:
    explicit HIDAPIWrapper(quint16 vid, quint16 m_pid, size_t inputBufferSize = INPUT_BUFFER_SIZE, size_t outpuBufferSize = OUTPUT_BUFFER_SIZE, QObject *parent = 0);

    ~HIDAPIWrapper();

    void receivedAsyncCallBack();

signals:
    void dataReceived(const unsigned char *receivedData, size_t lenght);

    void sendComplete(size_t lastPacketSentSize);
    void senrErr();

public slots:
    void sendData(const QByteArray &toSendData);
    void configurePollingTimer(int checkInterval);

    bool configureAsyncTransfer(void);

    void pollingTimerTimeout(void);

private:
    quint16 m_vid;
    quint16 m_pid;

    bool m_isOpen;
    bool m_asyncConfigured;

    //
    size_t m_inputBufferSize;
    size_t m_outputBufferSize;

    quint16 m_currentSendIndex;

    unsigned char *m_inputBuffer;
    unsigned char *m_outputBuffer;

    int m_lastSentPacketSize;

    //
    QTimer *m_dataCheckTimer;

    //
    libusb_device_handle *m_devHandler;
    libusb_context *m_ctx;

    //
    libusb_transfer *m_receptionTransferHandler;
    libusb_transfer *m_transmissionTransferHandler;

    //---- Constantes de operação.
    static const size_t INPUT_BUFFER_SIZE = 64;
    static const size_t OUTPUT_BUFFER_SIZE = 64;


    static const size_t MAX_OUTPUT_SIZE = 64;

    //----- ENDPOINTS
    static const unsigned char INTERRUPT_OUT_ENDPOINT = 0x01;
    static const unsigned char INTERRUPT_IN_ENDPOINT = 0x81;
};

extern "C" void receivedAsyncData(struct libusb_transfer *transfer);
#endif // HIDAPIWRAPPER_H
