#ifndef WATERFALLPLOT_H
#define WATERFALLPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>

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

};

#endif // WATERFALLPLOT_H
