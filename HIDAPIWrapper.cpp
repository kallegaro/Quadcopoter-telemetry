#include "HIDAPIWrapper.h"

HIDAPIWrapper::HIDAPIWrapper(quint16 vid, quint16 pid, size_t inputBufferSize, size_t outpuBufferSize, QObject *parent) :
    QObject(parent),
    m_vid(vid),
    m_pid(pid),
    m_isOpen(false),
    m_inputBufferSize(inputBufferSize),
    m_outputBufferSize(outpuBufferSize),
    m_currentSendIndex(0),
    m_inputBuffer(new QByteArray(m_inputBufferSize, 0x00)),
    m_outputBuffer(new QByteArray(m_outputBufferSize, 0x00)),
    m_lastSentPacketSize(0),
    m_dataCheckTimer(NULL)
{
    int r;

    r = libusb_init(&m_ctx);

    if( r < 0 ) {
         qFatal("Erro, não pode abrir o contexto");
    }else {
        m_isOpen = true;
    }

    libusb_set_debug(m_ctx, 3); //Seta a "verbosidade" do debug;

    m_devHandler = libusb_open_device_with_vid_pid(m_ctx, m_vid, m_pid);

    if(m_devHandler == NULL) {
        qFatal("Não foi possível abrir o dispositivo");
        m_isOpen = false;
    }

    r = libusb_detach_kernel_driver(m_devHandler, 0);

    switch(r) {
    case 0:
        qDebug() << "Driver Kernel desativado";
        break;

    case LIBUSB_ERROR_NOT_FOUND:
        qDebug() << "Driver do Kernel não estava ativo";
        break;

    case LIBUSB_ERROR_INVALID_PARAM:
        qDebug() << "Interface não existente";
        break;

    case LIBUSB_ERROR_NO_DEVICE:
        qDebug() << "Dispositivo desconectado";
        break;

    default:
        qDebug() << "Erro desconhecido: " << r;
        m_isOpen = false;
        break;

    }

    r = libusb_claim_interface(m_devHandler, 0);

    if( r < 0 ) {
        qDebug() << "Cannot Claim Interface";
        m_isOpen = false;
    }else {
        qDebug() << "Interface claimed";
        m_isOpen &= true;
    }

//    r = libusb_interrupt_transfer(m_devHandler, 0x01, data, 1, &lenght, 1000);
//    if(r >= 0) {
//        qDebug() << "Dados enviados: " << lenght;
//    }

//    r = libusb_interrupt_transfer(m_devHandler, 0x81, data, 64, &lenght, 1000);

//    if(r >= 0) {
//        if(lenght > 0) {
//            qDebug() << "Li alguma coisa: " << lenght;
//        }else {
//            qDebug() << "Não consegui li nada";
//        }
//    }else if( r < 0) {
//        qDebug() << "Erro na leitura: " << r;
//    }
}

void HIDAPIWrapper::sendData(const QByteArray &toSendData)
{
    unsigned char *data = new unsigned char[toSendData.size()];

    if(toSendData.size() < static_cast<int>(MAX_OUTPUT_SIZE)) {

        memcpy(data, toSendData.constData(), toSendData.size());

        int r = libusb_bulk_transfer(m_devHandler, INTERRUPT_OUT_ENDPOINT, data, toSendData.size(), &m_lastSentPacketSize, 50);

        if(r >= 0) {
            //qDebug() << "Dados enviados. número de bytes enviados: " << m_lastSentPacketSize;
            emit sendComplete(m_lastSentPacketSize);
        }else {
            qDebug() << "Dados não enviados, erro: " << r;
            emit senrErr();
        }
    }else {

    }

    delete data;
}

void HIDAPIWrapper::configurePollingTimer(int checkInterval)
{
    m_dataCheckTimer = new QTimer(this);

    connect(m_dataCheckTimer, &QTimer::timeout, this, &HIDAPIWrapper::pollingTimerTimeout);

    m_dataCheckTimer->start(checkInterval);
}

void HIDAPIWrapper::pollingTimerTimeout()
{
    unsigned char *data = new unsigned char[m_inputBufferSize];
    int lenght = 0;

    int r = libusb_interrupt_transfer(m_devHandler, INTERRUPT_IN_ENDPOINT, data, m_inputBufferSize, &lenght, 50);

    if(r >= 0) {
        if( lenght > 0 ) {
            m_inputBuffer->clear();
            m_inputBuffer->insert(0, (const char*)(data), lenght);

            //qDebug() << "Teste: " << data;
            emit dataReceived(*m_inputBuffer);
        }else {
            qDebug() << "Sem dados para recuperar";
        }
    }else {
        qDebug() << "Problema de recepção";
    }
//    if(r >= 0) {
//        if(lenght > 0) {
//            qDebug() << "Li alguma coisa: " << lenght;
//        }else {
//            qDebug() << "Não consegui li nada";
//        }
//    }else if( r < 0) {
//        qDebug() << "Erro na leitura: " << r;
//    }

    delete(data);
}

HIDAPIWrapper::~HIDAPIWrapper()
{
    libusb_release_interface(m_devHandler, 0);
    libusb_close(m_devHandler);

    libusb_exit(m_ctx);
}
