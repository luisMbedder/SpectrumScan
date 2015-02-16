/********************************************************************
* Project: SpectrumScan
*
* File: waterfallplot.h
*
* Created by: LuisMbedder
*
* Description: Contains all vairalbes
* and methods to generate the waterfall plot.
*
* notes:
********************************************************************/

#ifndef WATERFALLPLOT_H
#define WATERFALLPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "waterfalldata.h"
#include "Sdrcapture.h"

#define FFT_HISTORY (200) //200 fft's produces a nice resolution
#define WATERFALL_AREA (FFT_LENGTH*FFT_HISTORY)


class Waterfallplot : public QwtPlot
{
    Q_OBJECT

public:
    enum ColorMap
    {
        RGBMap,
        IndexMap,
        HueMap,
        AlphaMap
    };

    explicit Waterfallplot(QWidget *parent = 0);
    //sets the x-axis frequency range
    void SetFrequencyRange(double StartFreq,double StopFreq);
    double GetStartFrequency()const;
    double GetStopFrequency()const;
    //adds new fft data to the waterfall data buffer
    void PlotNewData(double* dataPoints);
    QTime upTime();
    void Reset();

signals:

public slots:
    void showContour(bool on);
    void showWaterfall(bool on);
    void setColorMap(int);
    void setAlpha(int);
    void printPlot();

private:
    QwtPlotSpectrogram *d_waterfall;
    int d_mapType;
    int d_alpha;
    double _startFrequency;
    double _stopFrequency;
    WaterfallData * waterfallData;

};

#endif // WATERFALLPLOT_H
