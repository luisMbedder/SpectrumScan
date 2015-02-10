#include "waterfalldata.h"
#include "Sdrcapture.h"
#include <qwt_matrix_raster_data.h>
#include "waterfallplot.h"
WaterfallData::WaterfallData(
                              double minimumFrequency,
                              double maximumFrequency,
                              double historyExtent
                               ): QwtMatrixRasterData()//public QwtRasterData
{

    _fftPoints = FFT_LENGTH;
    _historyLength = historyExtent;
    _spectrumData = new double[_fftPoints*_historyLength];
            setInterval( Qt::XAxis, QwtInterval( minimumFrequency, maximumFrequency ) );
            setInterval( Qt::YAxis, QwtInterval( 0, historyExtent ) );
            setInterval( Qt::ZAxis, QwtInterval( -90.0, -20.0 ) );


        //     setValueMatrix(fftData,1024);
     //       int columns = numColumns();
     //       int row = this->numRows();
    //         QVector<double> matrix = valueMatrix();
           //  qDebug()<<matrix.first();
          //   qDebug()<<matrix.last();
           //  int a=0;
             //replot();
            Reset();


}

void WaterfallData::Reset()
{
  memset(_spectrumData, 0x0, _fftPoints*_historyLength*sizeof(double));

  _numLinesToUpdate = -1;
}

void WaterfallData::UpdateInterval(double startFreq,double stopFreq){
    setInterval( Qt::XAxis, QwtInterval( startFreq, stopFreq ) );

}

void WaterfallData::ResizeData(double startFreq,
                   double stopFreq){

    setInterval(Qt::XAxis, QwtInterval(startFreq, stopFreq));



}

void WaterfallData::addFFTData(double *fftData){
    double result[FFT_LENGTH];

    //double result2[1024];
/*    for(int i =0;i<1024;i++)
    {
        result[i]=*fftData;
        fftData++;
    }*/
    memcpy(&result, fftData,FFT_LENGTH*sizeof(double));

    std::vector<double> rasterData;
    QVector<double> rasterVector(FFT_LENGTH);
    //static QVector<double> rV(FFT_LENGTH);
    int size = sizeof(result)/sizeof(*result);
    rasterData.assign(result,result+sizeof(result)/sizeof(*result));
    qCopy(result,result+size,rasterVector.begin());
       //rV.push_back(rasterVector);
// _historyLength++;
    setValueMatrix(rasterVector,FFT_LENGTH);
   int columns = numColumns();
   int row = this->numRows();
    QVector<double> matrix = valueMatrix();
    //QRectF pixel = pixelHint();
    double val = value(DEFAULT_CENTER_FREQUENCY/1000000.0 -1.2,60);
   //  qDebug()<<matrix.first();
    // qDebug()<<matrix.last();

}

double WaterfallData::value(double x, double y) const
{
 double returnValue = -90.0;
 QVector<double> matrix = valueMatrix();
 //QRectF pixel = pixelHint();
 //double val = value(DEFAULT_CENTER_FREQUENCY/1000000.0 -1.2,60);
  qDebug()<<matrix.first();
  qDebug()<<matrix.last();

#if QWT_VERSION < 0x060000
  const unsigned int intY = static_cast<unsigned int>((1.0 - (y/boundingRect().height())) *
                              static_cast<double>(_historyLength-1));
  const unsigned int intX = static_cast<unsigned int>((((x - boundingRect().left()) / boundingRect().width()) *
                               static_cast<double>(_fftPoints-1)) + 0.5);
#else
  double height = interval(Qt::YAxis).maxValue();
  double left = interval(Qt::XAxis).minValue();
  double right = interval(Qt::XAxis).maxValue();
  double ylen = static_cast<double>(_historyLength-1);
  double xlen = static_cast<double>(_fftPoints-1);
  const unsigned int intY = static_cast<unsigned int>((1.0 - y/height) * ylen);
  const unsigned int intX = static_cast<unsigned int>((((x - left) / (right-left)) * xlen) + 0.5);
#endif

  const int location = (intY * _fftPoints) + intX;
  if((location > -1) && (location < static_cast<int64_t>(_fftPoints * _historyLength))){
    returnValue = _spectrumData[location];
  }

  return returnValue;
}
