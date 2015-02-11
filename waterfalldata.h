#ifndef WATERFALLDATA_H
#define WATERFALLDATA_H
#include<qwt_raster_data.h>
#include<qwt_matrix_raster_data.h>
#include <inttypes.h>


class WaterfallData: public QwtMatrixRasterData
{
public:
    WaterfallData( double minimumFrequency,
                   double maximumFrequency,
                   uint64_t fftPoints,
                   unsigned int historyExtent);

    void UpdateInterval(double startFreq,double stopFreq);
    void addFFTData(double *fftData);
    void ResizeData(double startFreq, double stopFreq);
     virtual QwtRasterData *copy() const;
    virtual void Reset();
  virtual void Copy(const WaterfallData*);
  virtual double value(double x, double y) const;
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
