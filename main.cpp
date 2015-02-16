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
* Copyright (C) 2015 Luis Marquez <http://luismbedder.github.io>

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
#include "gui.h"
#include "waterfallplot.h"

int main(int argc, char *argv[])
{
    QApplication prog(argc, argv);
    Gui gui;
    gui.setWindowState(Qt::WindowFullScreen);
    gui.show();

    return prog.exec();
}
