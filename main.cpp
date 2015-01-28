/********************************************************************
* Project: SpectrumScan
* File: main.cpp
*
* Created By: LuisMbedder
*
* Description: Main module
*
* Known Issues: None
*
* Original Compiler: arm-linux-gnueabihf-gcc
* Original IDE: Qt Creator 3.1.1
* Toolchain: linaro arm
* Built on Ubuntu(x86) 14.10 machine for armv7 target.
********************************************************************/

#include <QApplication>
#include <QWSServer>
#include "gui.h"
#include "waterfallplot.h"
//#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication prog(argc, argv);
    //Gui gui;
    Waterfallplot waterfall;
    waterfall.setWindowState(Qt::WindowFullScreen);
    waterfall.showWaterfall(true);
   //  setCentralWidget(waterfall);
   //gui.setWindowState(Qt::WindowFullScreen);
   // gui.show();

    return prog.exec();
}
