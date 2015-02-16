/********************************************************************
* Project: SpectrumScan
*
* File: waterfalldata.cpp
*
* Written by: LuisMbedder
*
* Description: This class has all the methods/varialbes for the waterfall data
* processing. See waterfalldata.h for more info.
*
* notes:
********************************************************************/

#include "waterfalldata.h"
#include "Sdrcapture.h"
#include <qwt_matrix_raster_data.h>
#include "waterfallplot.h"


WaterfallData::WaterfallData(
                              double minimumFrequency,
                              double maximumFrequency,
                              uint64_t fftPoints,
                              unsigned int historyExtent
                               ): QwtMatrixRasterData()
{

    _fftPoints = fftPoints;
    _historyLength = historyExtent;
    _spectrumData = new double[_fftPoints*_historyLength];
    setInterval( Qt::XAxis, QwtInterval( minimumFrequency, maximumFrequency ) );
    setInterval( Qt::YAxis, QwtInterval( 0, historyExtent ) );
    setInterval( Qt::ZAxis, QwtInterval( MIN_INTENSITY, MAX_INTENSITY ) );
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

void WaterfallData::ResizeData(double startFreq,
                   double stopFreq){

    if((interval(Qt::XAxis).width() != (stopFreq - startFreq)) ||
       (interval(Qt::XAxis).minValue() != startFreq))
    {

      setInterval(Qt::XAxis, QwtInterval(startFreq, stopFreq));

    }

    Reset();

}

uint64_t WaterfallData::GetNumFFTPoints() const
{
    return _fftPoints;
}

void WaterfallData::addFFTData(double *fftData){

    int heightOffset = _historyLength - 1;

    if(heightOffset < 0){
      heightOffset = 0;
    }

    // Copy the old data over if any available
    if(heightOffset > 0){
      memmove( _spectrumData, &_spectrumData[_fftPoints],
           heightOffset * _fftPoints * sizeof(double)) ;
    }

    // add the new buffer
    memcpy(&_spectrumData[(_historyLength - 1) * _fftPoints], fftData,
       _fftPoints*sizeof(double));
}

double WaterfallData::value(double x, double y) const
{
  double returnValue = -90.0;

  double height = interval(Qt::YAxis).maxValue();
  double left = interval(Qt::XAxis).minValue();
  double right = interval(Qt::XAxis).maxValue();
  double ylen = static_cast<double>(_historyLength-1);
  double xlen = static_cast<double>(_fftPoints-1);
  const unsigned int intY = static_cast<unsigned int>((1.0 - y/height) * ylen);
  const unsigned int intX = static_cast<unsigned int>((((x - left) / (right-left)) * xlen) + 0.5);

  const int location = (intY * _fftPoints) + intX;
    //check if valid location
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


