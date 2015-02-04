/********************************************************************
* Project: SpectrumScan
*
* File: Spectrograph.cpp
*
* Description: This class creates the plot where the
*              RF spectrum will be displayed in real time.
*
* notes:
********************************************************************/

#include "spectrograph.h"
#include <qevent.h>

Spectrograph::Spectrograph(QWidget *parent):
    QwtPlot(parent)
{
    //real-time plotting
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot( false );
    //all plot widgets will be displayed on the canvas
    setCanvas( new QwtPlotCanvas() );

    plotLayout()->setAlignCanvasToScales( true );
    //set plot title and range
    setAxisTitle( QwtPlot::xBottom, "Frequency (MHz)" );
    setAxisTitle( QwtPlot::yLeft, "Amplitude (dB)" );
    setAxisScale( QwtPlot::xBottom, 0, RESULT_LENGTH );
    setAxisScale( QwtPlot::yLeft, -90, -10);

    //black background
    setCanvasBackground(QColor(0,0,0));

    //add grid
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::white, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );//attach grid to plot

    //add curve
    d_curve = new QwtPlotCurve();
    d_curve->setStyle( QwtPlotCurve::Lines );
    d_curve->setPen(Qt::yellow);
    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    d_curve->attach( this );//attach curve to plot

    for(int i=0; i<RESULT_LENGTH; ++i) {
        d_x[i] = i;
    }

}

void Spectrograph::SetData(double data[])
{
    // Do Frequency wrap here
    // Original FFT result
    // Bin (0) ..... Bin (N/2-1),   Bin (N/2), ....., Bin(N-1)
    // 0, ...........(Fs/2)-(Fs/N), Fs/2, .........., Fs-(Fs/N)
    // 0, ...........(Fs/2)-(Fs/N), -Fs/2,.........., -Fs/N
    // After this wrap
    // Bin (N/2), ....., Bin(N-1), Bin (0) ..... Bin (N/2-1)
    // -Fs/2,     ...., Fs-(Fs/N), 0       ..... (Fs/2)-(Fs/N)

    double temp;
    for (int i = 0; i <  RESULT_LENGTH/2; i++) {
        temp = data[RESULT_LENGTH/2 + i];
         data[RESULT_LENGTH/2 + i] = data[i];
         data[i] = temp;
    }
    int size = sizeof(data)/sizeof(data[0]);
    d_curve->setSamples(d_x, data, RESULT_LENGTH);
    replot();

}



void Spectrograph::SetXRange(double xStart, double xStop)
{
    setAxisScale( QwtPlot::xBottom, xStart, xStop );
    int num_points = RESULT_LENGTH;
    for(int i=0; i<num_points; ++i) {
        d_x[i] = xStart + (i/(num_points-1.0))* (xStop-xStart);
    }
}

