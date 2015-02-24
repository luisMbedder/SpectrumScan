#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T19:31:51
#
#-------------------------------------------------

QT       += core gui

include (/home/luis/qt-arm/qwt-6.1.0/qwt.prf)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpectrumScan
target.files=SpectrumScan
target.path = /home/root
INSTALLS +=target
TEMPLATE = app


SOURCES += main.cpp\
    gui.cpp \
    spectrograph.cpp \
    Sdrcapture.cpp \
    waterfallplot.cpp \
    qwt_double_rect.cpp \
    qwt_double_interval.cpp \
    waterfalldata.cpp

HEADERS  += \
    gui.h \
    spectrograph.h \
    Sdrcapture.h \
    waterfallplot.h \
    qwt_double_rect.h \
    qwt_double_interval.h \
    waterfalldata.h

FORMS    += \
    gui.ui

INCLUDEPATH += /home/luis/qt-arm/rtl-sdr/include
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/i386-linux-gnu
#LIBS += -lusb-1.0  -lrtlsdr -lavdevice -lavfilter -lavformat -lavresample -lavcodec -lswscale -lavutil
LIBS += -L/home/luis/qt-bbb/libusb/lib -lusb-1.0  -L/home/luis/qt-bbb/librtlsdr-arm -lrtlsdr -L/usr/local/lib -lavdevice -lavfilter -lavformat -lavresample -lavcodec -lswscale -lavutil

QMAKE_CXXFLAGS += -Wno-psabi -D__STDC_CONSTANT_MACROS

RESOURCES += \
    Resources.qrc
