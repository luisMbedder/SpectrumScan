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
    waterfalldata.cpp \
    newwaterfallplot.cpp

HEADERS  += \
    gui.h \
    spectrograph.h \
    Sdrcapture.h \
    waterfallplot.h \
    qwt_double_rect.h \
    qwt_double_interval.h \
    waterfalldata.h \
    newwaterfallplot.h

FORMS    += \
    gui.ui

INCLUDEPATH += /home/luis/qt-arm/rtl-sdr/include
LIBS += -L/qt-bbb/tslib/lib -lts
LIBS += -L/home/luis/qt-bbb/libusb/lib -lusb-1.0  -L/home/luis/qt-bbb/librtlsdr-arm -lrtlsdr -L/usr/local/lib -lavdevice -lavfilter -lavformat -lavresample -lavcodec -lswscale -lavutil
INCLUDEPATH += /usr/include
//INCLUDEPATH += /opt/libav/include
INCLUDEPATH += /usr/include/i386-linux-gnu
OTHER_FILES +=
QMAKE_CXXFLAGS += -Wno-psabi -D__STDC_CONSTANT_MACROS
