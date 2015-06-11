#include "HIDAPIWrapper.h"

HIDAPIWrapper::HIDAPIWrapper(quint16 vid, quint16 pid, size_t inputBufferSize, size_t outpuBufferSize, QObject *parent) :
    QObject(parent),
    m_vid(vid),
    m_pid(pid),
    m_isOpen(false),
    m_asyncConfigured(false),
    m_inputBufferSize(inputBufferSize),
    m_outputBufferSize(outpuBufferSize),
    m_currentSendIndex(0),
    m_inputBuffer(new unsigned char[INPUT_BUFFER_SIZE]),
    m_outputBuffer(new unsigned char[OUTPUT_BUFFER_SIZE]),
    m_lastSentPacketSize(0),
    m_dataCheckTimer(NULL),
    m_receptionTransferHandler(NULL),
    m_transmissionTransferHandler(NULL)
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

}

void HIDAPIWrapper::sendData(const QByteArray &toSendData)
{
    if(toSendData.size() < static_cast<int>(MAX_OUTPUT_SIZE)) {

        memcpy(m_outputBuffer, toSendData.constData(), toSendData.size());

        int r = libusb_interrupt_transfer(m_devHandler, INTERRUPT_OUT_ENDPOINT, m_outputBuffer, toSendData.size(), &m_lastSentPacketSize, 100);

        if(r >= 0) {
            //qDebug() << "Dados enviados. número de bytes enviados: " << m_lastSentPacketSize;
            emit sendComplete(m_lastSentPacketSize);
        }else {
            qDebug() << "Dados não enviados, erro: " << r;
            emit senrErr();
        }
    }else {

    }
}

void HIDAPIWrapper::configurePollingTimer(int checkInterval)
{
    m_dataCheckTimer = new QTimer(this);

    connect(m_dataCheckTimer, &QTimer::timeout, this, &HIDAPIWrapper::pollingTimerTimeout);

    m_dataCheckTimer->start(checkInterval);
}

void HIDAPIWrapper::pollingTimerTimeout(void)
{
    int lenght = 0;

    int r = libusb_interrupt_transfer(m_devHandler, INTERRUPT_IN_ENDPOINT, m_inputBuffer, m_inputBufferSize, &lenght, 50);

    if(r >= 0) {
        if( lenght > 0 ) {
            //qDebug() << "Teste: " << data;
            emit dataReceived(m_inputBuffer, lenght);
        }
    }
}

HIDAPIWrapper::~HIDAPIWrapper()
{
    libusb_release_interface(m_devHandler, 0);
    libusb_close(m_devHandler);

    libusb_exit(m_ctx);

    //----- Desaloca os recursos utilizados quando se realizou uma transferência
    if (m_asyncConfigured) {
        libusb_cancel_transfer(m_receptionTransferHandler);
        libusb_free_transfer(m_receptionTransferHandler);
    }

}

bool HIDAPIWrapper::configureAsyncTransfer()
{
    m_receptionTransferHandler = libusb_alloc_transfer(0);

    libusb_fill_interrupt_transfer(m_receptionTransferHandler, m_devHandler, INTERRUPT_IN_ENDPOINT, m_inputBuffer, INPUT_BUFFER_SIZE, receivedAsyncData, this, 500);

    int r = libusb_submit_transfer(m_receptionTransferHandler);

    if(r >= 0) {
        qDebug() << "Transferência configurada com suncesso";
        m_asyncConfigured = true;
    }

    return m_asyncConfigured;
}

void receivedAsyncData(libusb_transfer *transfer)
{
    HIDAPIWrapper &wrapper = *static_cast<HIDAPIWrapper *>(transfer->user_data);

    wrapper.receivedAsyncCallBack();
}

void HIDAPIWrapper::receivedAsyncCallBack()
{
    libusb_submit_transfer(m_receptionTransferHandler);

    if (m_receptionTransferHandler->status == LIBUSB_TRANSFER_COMPLETED)
        emit dataReceived(m_inputBuffer, m_receptionTransferHandler->actual_length);
}
