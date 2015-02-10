#ifndef WATERFALLDATA_H
#define WATERFALLDATA_H
#include<qwt_raster_data.h>
#include<qwt_matrix_raster_data.h>



class WaterfallData: public QwtMatrixRasterData
{
public:
    WaterfallData( double minimumFrequency,
                   double maximumFrequency,
                   double historyExtent);

    void UpdateInterval(double startFreq,double stopFreq);
    void addFFTData(double *fftData);
    void ResizeData(double startFreq, double stopFreq);
    virtual void Reset();
 // virtual void Copy(const WaterfallData*);
  virtual double value(double x, double y) const;
protected:
        double* _spectrumData;
        int _fftPoints;
        int _historyLength;
        int _numLinesToUpdate;

};

#endif // WATERFALLDATA_H
