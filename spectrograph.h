/********************************************************************
* Project: SpectrumScan
*
* File: Spectrograph.cpp
*
* Description: This file defines the spectrogrpah class
*              used to display the RF spectrum in real time.
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


/**************************************************************
 * Class name: Spectrograph
 *
 * Created by: LuisMbedder
 *
 * Description: This class creates the plot to
 *              display the spectrum.
 *
 * Notes:
 **************************************************************/


class Spectrograph : public QwtPlot
{
    Q_OBJECT
public:

    /*
    * Constructor: Spectrograph
    * Usage: Spectrograph plot = new Spectrograph(parent);
    *        Spectrograph plot = new Spectrograph();
    * ---------------------------
    * creates a new Spectrograph plot.
    */
    explicit Spectrograph(QWidget *parent = 0);

    /*
    * Method: SetData
    * Usage: spectrograph->SetData(data);
    * ---------------------------
    * sets the curves data on the plot according to
    * the data array which is the FFT result in dB. data is
    * the y-axis values, or the intesnisty of the signal. The
    * x-values are set using the SetXRange method.
    */
    void SetData(double data[]);

    /*
    * Method: SetXRange
    * Usage: spectrogram->SetXRange(xStart,xStop)
    * ---------------------------
    * Sets the frequency range on the x-axis of the plot and
    * calculates the frequency resolution or bin spacing of the
    * data. xStart is the starting frequency, and xStop is the
    * ending frequency.
    */
    void SetXRange(double xStart, double xStop);

signals:

public slots:

private:
    QwtPlotDirectPainter *d_directPainter;
    QwtPlotCurve *d_curve;
    //array to hold the frequency interval or bin spacing
    double d_x[RESULT_LENGTH];

};

#endif // SPECTROGRAPH_H
