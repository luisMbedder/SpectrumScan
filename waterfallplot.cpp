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

class SpectrogramData: public QwtMatrixRasterData//public QwtRasterData
{
public:
    SpectrogramData(QVector<double> data)
   // SpectrogramData()
    {
       //  this->
      //  int columns = QwtMatrixRasterData::numColumns();
        //int row = this->numRows();
       // QVector<double> matrix = valueMatrix();
    //   double sf=Waterfallplot->GetStartFrequency();
        setInterval( Qt::XAxis, QwtInterval( 950.75, 953.2 ) );
       setInterval( Qt::YAxis, QwtInterval( 0, 5 ) );
       setInterval( Qt::ZAxis, QwtInterval( -90.0, -10.0 ) );


         setValueMatrix(data,1024);
        int columns = numColumns();
        int row = this->numRows();
         QVector<double> matrix = valueMatrix();
       //  qDebug()<<matrix.first();
      //   qDebug()<<matrix.last();
       //  int a=0;
         //replot();

    }

    SpectrogramData()
   // SpectrogramData()
    {
       //  this->
      //  int columns = QwtMatrixRasterData::numColumns();
        //int row = this->numRows();
       // QVector<double> matrix = valueMatrix();
        setInterval( Qt::XAxis, QwtInterval( 950.75, 953.2 ) );
        setInterval( Qt::YAxis, QwtInterval( 0, 5 ) );
        setInterval( Qt::ZAxis, QwtInterval( -90.0, -10.0 ) );


      //   setValueMatrix(values,1024);
     //   int columns = numColumns();
      //  int row = this->numRows();
      //   QVector<double> matrix = valueMatrix();

    }

  /*  virtual double value( double x, double y ) const
    {
        //const double c = 0.842;
        const double c = 0.33;
        if(x>1.48){
            int a =0;
        }
        if(y<-1.48){
            int b =0;
        }
        const double v1 = x * x + ( y - c ) * ( y + c );
        const double v2 = x * ( y + c ) + x * ( y + c );

        int columns = numColumns();
        int row = this->numRows();
        //setValueMatrix(data,1024);
         QVector<double> matrix = valueMatrix();

        return 1.0 / ( v1 * v1 + v2 * v2 );

    }*/
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
        QwtLinearColorMap( Qt::darkBlue, Qt::white, QwtColorMap::RGB )
    {
        addColorStop( 0.2, Qt::blue );
        addColorStop( 0.4, Qt::cyan );
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

Waterfallplot::Waterfallplot( QWidget *parent ):
    QwtPlot( parent ),
    d_alpha(255)
{

    _startFrequency =  DEFAULT_CENTER_FREQUENCY/1000000.0 -1.2;
    _stopFrequency =  DEFAULT_CENTER_FREQUENCY/1000000.0 +1.2;

    d_waterfall = new QwtPlotSpectrogram();
    d_waterfall ->setRenderThreadCount( 0 ); // use system specific thread count
    d_waterfall ->setCachePolicy( QwtPlotRasterItem::PaintCache );
    waterfallData = new WaterfallData(_startFrequency,_stopFrequency);
    d_waterfall->setData(waterfallData);
   // d_waterfall->setData( new SpectrogramData() ); //commented
    d_waterfall->attach( this );

   const QwtInterval zInterval = d_waterfall->data()->interval( Qt::ZAxis );//commented

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() ); //commented
    enableAxis( QwtPlot::yRight );

    setAxisScale(QwtPlot::xBottom,950.75,953.2);
    setAxisMaxMinor(QwtPlot::xBottom,0);
    setAxisScale(QwtPlot::yLeft,0,5);
    setAxisMaxMinor(QwtPlot::yLeft,0);

    plotLayout()->setAlignCanvasToScales( true );

   setColorMap( Waterfallplot::RGBMap ); //commented

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );
}

void Waterfallplot::SetWaterfallData(QVector<double> rasterVector){
//SpectrogramData dat = new SpectrogramData();
   // std::vector<double> rasterData;
  //  int size = sizeof(data)/sizeof(*data);

    d_waterfall->setData( new SpectrogramData(rasterVector) );
    replot();
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
