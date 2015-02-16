/********************************************************************
* Project: SpectrumScan
*
* File: waterfalldata.h
*
* Created by: LuisMbedder
*
* Description: This interface has all the methods for the WaterfallData
* processing.
*
* notes:
********************************************************************/

#ifndef WATERFALLDATA_H
#define WATERFALLDATA_H

#include<qwt_raster_data.h>
#include<qwt_matrix_raster_data.h>
#include <inttypes.h>
#define MIN_INTENSITY (-90.0)
#define MAX_INTENSITY (-20.0)

class WaterfallData: public QwtMatrixRasterData
{
public:
    /*
    * Constructor: WaterfallData
    * Usage: WaterfallData data = new WaterfallData(parent);
    * ---------------------------
    * Sets the waterfall plots x,y, and z intervals using the
    * minimumFrequency, maximumFrequency, and historyExtent parameters.
    * fftPoints is the length of the FFT, in this case hard coded to 1024.
    */
    WaterfallData( double minimumFrequency,
                   double maximumFrequency,
                   uint64_t fftPoints,
                   unsigned int historyExtent);

    virtual ~WaterfallData();
    /*
    * Adds a new FFT block to the _spectrumData buffer by
    * shifting the oldest FFT block out and adding the new FFT block to
    * the end of the buffer.
    */
    void addFFTData(double *fftData);
    //updates the x-axis interval
    void ResizeData(double startFreq, double stopFreq);
    //resets the spectrum data buffer to 0
    virtual void Reset();
    /*
    * implements QwtRasterData::value() method and
    * returns the intensity of the spectrum according to
    * the location calcluated using the x,y values. This
    * method is called once for each pixel on the plot canvas.
    */
    virtual double value(double x, double y) const;
    //returns a pointer to the spectrumData buffer
    virtual double* GetSpectrumDataBuffer()const;
    virtual void SetSpectrumDataBuffer(const double*);
    virtual int GetNumLinesToUpdate()const;
    virtual void SetNumLinesToUpdate(const int);
    virtual void IncrementNumLinesToUpdate();
    virtual uint64_t GetNumFFTPoints()const;

protected:
        double* _spectrumData;
        int _fftPoints;
        int _historyLength;
        int _numLinesToUpdate;


};

#endif // WATERFALLDATA_H
