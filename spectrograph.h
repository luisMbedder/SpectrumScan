/********************************************************************
* Project: SpectrumScan
*
* File: Spectrograph.cpp
*
* Description: This file defines the spectrogrpah class
*              to display the RF spectrum
*
*
* notes:
********************************************************************/

#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <stdint.h>
#include <QObject>
#include <qwt_plot.h>
#include <stdint.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include "Sdrcapture.h"


class Spectrograph : public QwtPlot
{
    Q_OBJECT
public:

    /*
    * Constructor: Spectrograph
    * Usage: Spectrograph ;
    * ---------------------------
    * creates a Spectrograph Object. The constructor
    */
    explicit Spectrograph(QWidget *parent = 0);

    /*
    * Method: SetData
    * Usage:
    * ---------------------------
    *
    */
    void SetData(double data[]);

    /*
    * Method: SetXRange
    * Usage:
    * ---------------------------
    *
    */
    void SetXRange(double xStart, double xStop);

signals:

public slots:

private:
    QwtPlotDirectPainter *d_directPainter;
    QwtPlotCurve *d_curve;
    double d_x[RESULT_LENGTH];

};

#endif // SPECTROGRAPH_H
