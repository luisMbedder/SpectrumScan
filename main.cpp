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
*
* Notes:
*
* SpectrumScan - RTL-SDR 2Ghz Spectrum Analyzer for the BeagleBone
* Copyright (C) 2015 Luis Marquez <http://luismbedder.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include <QApplication>
#include <QWSServer>
#include <QThread>
#include <QTimer>
#include <QSplashScreen>
#include <QPixmap>
#include <QMessageBox>
#include <QThread>
#include "gui.h"
#include "waterfallplot.h"

class MyInitThread : public QThread
{
protected:
    void run(void)
    {

        QThread::msleep(3500); //show splash for 3 seconds
    }
};



int main(int argc, char *argv[])
{
    QApplication prog(argc, argv);

  //  QPixmap pixmap(":/images/ss-splash-screen.png"); // Insert splash image
    QPixmap pixmap(":/images/ss-splash-v4.png"); // Insert splash image
    if(pixmap.isNull())
    {
        QMessageBox::warning(0, "Error", "Failed to load Splash Screen image!");
    }

    QSplashScreen splash(pixmap);
    splash.setEnabled(false);//prevent user from closing splash
    splash.show();

    prog.processEvents();//Make sure splash screen gets drawn ASAP

    QEventLoop loop;

    MyInitThread *thread = new MyInitThread();
    QObject::connect(thread, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(thread, SIGNAL(terminated()), &loop, SLOT(quit()));
    thread->start();

    loop.exec(); //Do event processing until the thread has finished
    Gui gui;
    gui.setWindowState(Qt::WindowFullScreen);
    gui.show();
    prog.processEvents();
    splash.hide();

    return prog.exec();

}
