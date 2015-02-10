#include <qprinter.h>
#include <qprintdialog.h>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include "waterfallplot.h"
#include "waterfalldata.h"
#include "Sdrcapture.h"
#include <QTime>

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};


void Waterfallplot::SetFrequencyRange(double StartFreq,
                    double StopFreq)
{
//  double startFreq = constStartFreq;
 // double stopFreq = constStopFreq ;

  bool reset = false;
  if((StartFreq != _startFrequency) || (StopFreq != _stopFrequency))
    reset = true;

  if(StopFreq > StartFreq) {
    _startFrequency =StartFreq;
    _stopFrequency = StopFreq;

    setAxisScale( QwtPlot::xBottom, StartFreq, StopFreq );

  //  setInterval( Qt::YAxis, QwtInterval( 0, 5 ) );
  //  setInterval( Qt::ZAxis, QwtInterval( -90.0, -10.0 ) );
//replot();
  }

}



double
Waterfallplot::GetStartFrequency() const
{
  return _startFrequency;
}

double
Waterfallplot::GetStopFrequency() const
{
  return _stopFrequency;
}

//sets the intensity color range
class LinearColorMapRGB: public QwtLinearColorMap
{
public:
    LinearColorMapRGB():
        QwtLinearColorMap( Qt::darkBlue, Qt::darkRed, QwtColorMap::RGB )
    {
        addColorStop( 0.2, Qt::blue );
        addColorStop( 0.3, Qt::cyan );
        addColorStop( 0.6, Qt::yellow );
        addColorStop( 0.8, Qt::red );

    }
};

class LinearColorMapIndexed: public QwtLinearColorMap
{
public:
    LinearColorMapIndexed():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }
};

class HueColorMap: public QwtColorMap
{
public:
    // class backported from Qwt 6.2

    HueColorMap():
        d_hue1(0),
        d_hue2(359),
        d_saturation(150),
        d_value(200)
    {
        updateTable();

    }

    virtual QRgb rgb( const QwtInterval &interval, double value ) const
    {
        if ( qIsNaN(value) )
            return 0u;

        const double width = interval.width();
        if ( width <= 0 )
            return 0u;

        if ( value <= interval.minValue() )
            return d_rgbMin;

        if ( value >= interval.maxValue() )
            return d_rgbMax;

        const double ratio = ( value - interval.minValue() ) / width;
        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

        if ( hue >= 360 )
        {
            hue -= 360;

            if ( hue >= 360 )
                hue = hue % 360;
        }

        return d_rgbTable[hue];
    }

    virtual unsigned char colorIndex( const QwtInterval &, double ) const
    {
        // we don't support indexed colors
        return 0;
    }


private:
    void updateTable()
    {
        for ( int i = 0; i < 360; i++ )
            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
    }

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};

class AlphaColorMap: public QwtAlphaColorMap
{
public:
    AlphaColorMap()
    {
        //setColor( QColor("DarkSalmon") );
        setColor( QColor("SteelBlue") );
    }
};

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addSecs( static_cast<int>( v ) );
        return upTime.toString();
    }
private:
    QTime baseTime;
};
/*
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addSecs( static_cast<int>( v ) );
        return upTime.toString();
    }
private:
    QTime baseTime;
};

*/
QTime Waterfallplot::upTime()
{
    QTime t( 0, 0, 0 );
  //  for ( int i = 0; i < 4; i++ )
   //    t = t.addSecs( int( procValues[i] / 100 ) );

    return t;
}

Waterfallplot::Waterfallplot( QWidget *parent ):
    QwtPlot( parent ),
    d_alpha(255)
{

    _startFrequency =  DEFAULT_CENTER_FREQUENCY/1000000.0 -1.2;
    _stopFrequency =  DEFAULT_CENTER_FREQUENCY/1000000.0 +1.2;

    QPalette palette;
    palette.setColor(canvas()->backgroundRole(), QColor("white"));
    canvas()->setPalette(palette);

    setAxisTitle(QwtPlot::xBottom, "Frequency (MHz)");

    setAxisTitle(QwtPlot::yLeft, "Time(s)");
    setAxisScaleDraw( QwtPlot::yLeft,
       new TimeScaleDraw( upTime() ) );
    setAxisScale( QwtPlot::yLeft, 0, 60 );

     ( void )startTimer( 1000 ); // 1 second

    d_waterfall = new QwtPlotSpectrogram();
    d_waterfall ->setRenderThreadCount( 0 ); // use system specific thread count
    d_waterfall ->setCachePolicy( QwtPlotRasterItem::PaintCache );
    waterfallData = new WaterfallData(_startFrequency,_stopFrequency,1.0);
    d_waterfall->setData(waterfallData);
    d_waterfall->attach( this );

   const QwtInterval zInterval = d_waterfall->data()->interval( Qt::ZAxis );//commented

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() ); //commented
    enableAxis( QwtPlot::yRight );

    setAxisScale(QwtPlot::xBottom,_startFrequency,_stopFrequency);
    setAxisMaxMinor(QwtPlot::xBottom,0);
  //  setAxisScale(QwtPlot::yLeft,0,50);
  //  setAxisMaxMinor(QwtPlot::yLeft,0);

    plotLayout()->setAlignCanvasToScales( true );

   setColorMap( Waterfallplot::RGBMap ); //commented

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size
/*
    QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );
*/

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );

   // const QColor c( Qt::darkBlue );
  //  zoomer->setRubberBandPen( c );
 //   zoomer->setTrackerPen( c );
}

void Waterfallplot::timerEvent( QTimerEvent * )
{

    int a=0;
   // waterfallData->setInterval(Qt::YAxis,QwtInterval(0,waterfallData->));
    a++;
    //PlotNewData();
/*    for ( int i = dataCount; i > 0; i-- )
    {





        for ( int c = 0; c < NCpuData; c++ )
        {
            if ( i < HISTORY )
                data[c].data[i] = data[c].data[i-1];
        }
    }

    cpuStat.statistic( data[User].data[0], data[System].data[0] );

    data[Total].data[0] = data[User].data[0] + data[System].data[0];
    data[Idle].data[0] = 100.0 - data[Total].data[0];

    if ( dataCount < HISTORY )
        dataCount++;

    for ( int j = 0; j < HISTORY; j++ )
        timeData[j]++;

    setAxisScale( QwtPlot::xBottom,
        timeData[HISTORY - 1], timeData[0] );

    for ( int c = 0; c < NCpuData; c++ )
    {
        data[c].curve->setRawSamples(
            timeData, data[c].data, dataCount );
    }

    replot();*/
    replot();
}

void Waterfallplot::PlotNewData(double* dataPoints){

  //  d_waterfall->invalidateCache();
  // d_waterfall->itemChanged();

    waterfallData->addFFTData(dataPoints);

    d_waterfall->invalidateCache();
   d_waterfall->itemChanged();
Reset();
  //  replot();

}

void Waterfallplot::Reset(){
    waterfallData->ResizeData(_startFrequency,_stopFrequency);
  //  waterfallData->Reset();

    setAxisScale(QwtPlot::xBottom, _startFrequency, _stopFrequency);


}


void Waterfallplot::showContour( bool on )
{
    d_waterfall->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void Waterfallplot::showWaterfall( bool on )
{
    d_waterfall->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_waterfall->setDefaultContourPen(
        on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    replot();
}

void Waterfallplot::setColorMap( int type )
{
    QwtScaleWidget *axis = axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = d_waterfall->data()->interval( Qt::ZAxis );

    d_mapType = type;

    int alpha = d_alpha;
    switch( type )
    {
        case Waterfallplot::HueMap:
        {
            d_waterfall->setColorMap( new HueColorMap() );
            axis->setColorMap( zInterval, new HueColorMap() );
            break;
        }
        case Waterfallplot::AlphaMap:
        {
            alpha = 255;
            d_waterfall->setColorMap( new AlphaColorMap() );
            axis->setColorMap( zInterval, new AlphaColorMap() );
            break;
        }
        case Waterfallplot::IndexMap:
        {
            d_waterfall->setColorMap( new LinearColorMapIndexed() );
            axis->setColorMap( zInterval, new LinearColorMapIndexed() );
            break;
        }
        case Waterfallplot::RGBMap:
        default:
        {
            d_waterfall->setColorMap( new LinearColorMapRGB() );
            axis->setColorMap( zInterval, new LinearColorMapRGB() );
        }
    }
    d_waterfall->setAlpha( alpha );

    replot();
}

void Waterfallplot::setAlpha( int alpha )
{
    // setting an alpha value doesn't make sense in combination
    // with a color map interpolating the alpha value

    d_alpha = alpha;
    if ( d_mapType != Waterfallplot::AlphaMap )
    {
        d_waterfall->setAlpha( alpha );
        replot();
    }
}

#ifndef QT_NO_PRINTER

void Waterfallplot::printPlot()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setOrientation( QPrinter::Landscape );
    printer.setOutputFileName( "spectrogram.pdf" );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( this, printer );
    }
}

#endif
