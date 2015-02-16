#include "waterfalldata.h"
#include "Sdrcapture.h"
#include <qwt_matrix_raster_data.h>
#include "waterfallplot.h"
WaterfallData::WaterfallData(
                              double minimumFrequency,
                              double maximumFrequency,
                              uint64_t fftPoints,
                              unsigned int historyExtent
                               ): QwtMatrixRasterData()//public QwtRasterData
{

    _fftPoints = fftPoints;
    _historyLength = historyExtent;
    _spectrumData = new double[_fftPoints*_historyLength];
            setInterval( Qt::XAxis, QwtInterval( minimumFrequency, maximumFrequency ) );
            setInterval( Qt::YAxis, QwtInterval( 0, historyExtent ) );
            setInterval( Qt::ZAxis, QwtInterval( MIN_INTENSITY, MAX_INTENSITY ) );


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

WaterfallData::~WaterfallData()
{
  delete[] _spectrumData;
}

void WaterfallData::Reset()
{
  memset(_spectrumData, 0x0, _fftPoints*_historyLength*sizeof(double));

  _numLinesToUpdate = -1;
}

void WaterfallData::UpdateInterval(double startFreq,double stopFreq){
    setInterval( Qt::XAxis, QwtInterval( startFreq, stopFreq ) );

}

void WaterfallData::Copy(const WaterfallData* rhs)
{

  if(_fftPoints != rhs->GetNumFFTPoints()) {
    _fftPoints = rhs->GetNumFFTPoints();
    delete[] _spectrumData;
    _spectrumData = new double[_fftPoints * _historyLength];
  }


  Reset();
  SetSpectrumDataBuffer(rhs->GetSpectrumDataBuffer());
  SetNumLinesToUpdate(rhs->GetNumLinesToUpdate());

  setInterval(Qt::XAxis, rhs->interval(Qt::XAxis));
  setInterval(Qt::YAxis, rhs->interval(Qt::YAxis));
  setInterval(Qt::ZAxis, rhs->interval(Qt::ZAxis));

}

QwtRasterData *WaterfallData::copy() const
{

  WaterfallData* returnData =  new WaterfallData(interval(Qt::XAxis).minValue(),
                         interval(Qt::XAxis).maxValue(),
                         _fftPoints, _historyLength);


  returnData->Copy(this);
  return returnData;
}


void WaterfallData::ResizeData(double startFreq,
                   double stopFreq){

    if((interval(Qt::XAxis).width() != (stopFreq - startFreq)) ||
       (interval(Qt::XAxis).minValue() != startFreq)){

      setInterval(Qt::XAxis, QwtInterval(startFreq, stopFreq));

    //  _fftPoints = fftPoints;
      delete[] _spectrumData;
      _spectrumData = new double[_fftPoints * _historyLength];
    }

    Reset();

}

uint64_t WaterfallData::GetNumFFTPoints() const
{
  return _fftPoints;
}

void WaterfallData::addFFTData(double *fftData){
    double result[FFT_LENGTH];


    int droppedFrames=0;
    int heightOffset = _historyLength - 1 - droppedFrames;
    uint64_t drawingDroppedFrames = droppedFrames;

    // Any valid data rolled off the display so just fill in zeros and write new data
    if(heightOffset < 0){
      heightOffset = 0;
      drawingDroppedFrames = static_cast<uint64_t>(_historyLength-1);
    }

    // Copy the old data over if any available
    if(heightOffset > 0){
      memmove( _spectrumData, &_spectrumData[(drawingDroppedFrames+1) * _fftPoints],
           heightOffset * _fftPoints * sizeof(double)) ;
    }

    if(drawingDroppedFrames > 0){
      // Fill in zeros data for dropped data
      memset(&_spectrumData[heightOffset * _fftPoints], 0x00,
         drawingDroppedFrames * _fftPoints * sizeof(double));
    }

    // add the new buffer
    memcpy(&_spectrumData[(_historyLength - 1) * _fftPoints], fftData,
       _fftPoints*sizeof(double));


    /*
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
    */

}

double WaterfallData::value(double x, double y) const
{
 double returnValue = -90.0;
 QVector<double> matrix = valueMatrix();
 //QRectF pixel = pixelHint();
 //double val = value(DEFAULT_CENTER_FREQUENCY/1000000.0 -1.2,60);
//  qDebug()<<matrix.first();
 // qDebug()<<matrix.last();
// QRectF area =


  double height = interval(Qt::YAxis).maxValue();
  double left = interval(Qt::XAxis).minValue();
  double right = interval(Qt::XAxis).maxValue();
  double ylen = static_cast<double>(_historyLength-1);
  double xlen = static_cast<double>(_fftPoints-1);
  const unsigned int intY = static_cast<unsigned int>((1.0 - y/height) * ylen);
  const unsigned int intX = static_cast<unsigned int>((((x - left) / (right-left)) * xlen) + 0.5);
QRect area(qreal(0),qreal(0),qreal(height),qreal(left-right));
   QRectF pixelGeom = pixelHint(area);

  const int location = (intY * _fftPoints) + intX;

  if((location > -1) && (location < WATERFALL_AREA)){
    returnValue = _spectrumData[location];
  }

  return returnValue;
}


double* WaterfallData::GetSpectrumDataBuffer() const
{
  return _spectrumData;
}

void WaterfallData::SetSpectrumDataBuffer(const double* newData)
{
  memcpy(_spectrumData, newData, _fftPoints * _historyLength * sizeof(double));
}

int WaterfallData::GetNumLinesToUpdate() const
{
  return _numLinesToUpdate;
}

void WaterfallData::SetNumLinesToUpdate(const int newNum)
{
  _numLinesToUpdate = newNum;
}

void WaterfallData::IncrementNumLinesToUpdate()
{
  _numLinesToUpdate++;
}


