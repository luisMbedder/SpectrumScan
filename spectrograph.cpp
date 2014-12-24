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

    setTitle("SpectrumScan - LuisMbedder");
    setAxisTitle( QwtPlot::xBottom, "Frequency (MHz)" );
    setAxisTitle( QwtPlot::yLeft, "Amplitude (dB)" );
    setAxisScale( QwtPlot::xBottom, 0, RESULT_LENGTH );
    setAxisScale( QwtPlot::yLeft, -90, -10);
    //setAutoFillBackground(true);
   // setCanvasBackground(QColor(29,100,141)); //blue color
    setCanvasBackground(QColor(0,0,0));
   // QwtScaleWidget *qwtsw = myqwtplot.axisWidget(QwtPlot::xBottom);
   // QPalette palette = this->palette();
  //  palette.setColor( QPalette::WindowText, Qt::yellow); // for ticks
  //  palette.setColor( QPalette::Text, Qt::yellow); // for ticks' labels
  //  this->setPalette( palette );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::white, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );


    d_curve = new QwtPlotCurve();
    d_curve->setStyle( QwtPlotCurve::Lines );
    d_curve->setPen(Qt::green);//Qt::yellow);
    //d_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    d_curve->attach( this );

    //for(int i=0; i<RESULT_LENGTH; ++i) d_x[i] = i;


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

