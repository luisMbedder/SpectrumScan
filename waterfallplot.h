#ifndef WATERFALLPLOT_H
#define WATERFALLPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "waterfalldata.h"

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

    void SetWaterfallData(QVector<double> rasterVector);
    void SetFrequencyRange(double StartFreq,
                        double StopFreq);
    double GetStartFrequency()const;
    double GetStopFrequency()const;
//void  UpdateInterval(double startFreq,double stopFreq);

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
