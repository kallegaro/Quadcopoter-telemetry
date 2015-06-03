#-------------------------------------------------
#
# Project created by QtCreator 2015-05-16T16:46:19
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtHIDTest
TEMPLATE = app


SOURCES += main.cpp\
        qthidtest.cpp \
    HIDAPIWrapper.cpp \
#    ../hidapi/libusb/hid.c
    QuadTelemetry.cpp \
    qcustomplot.cpp

HEADERS  += qthidtest.h \
    HIDAPIWrapper.h \
#    ../hidapi/hidapi/hidapi.h
    QuadTelemetry.h \
    qcustomplot.h

FORMS    += qthidtest.ui

LIBS += -L/usr/lib/x86_64-linux-gnu
LIBS += -lusb-1.0
#LIBS += ../hidapi/libusb/hid.o

INCLUDEPATH +=/usr/include/libusb-1.0
INCLUDEPATH +=../hidapi/hidapi/

OTHER_FILES += \
    ../hidapi/libusb/hid.o


