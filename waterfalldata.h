#ifndef WATERFALLDATA_H
#define WATERFALLDATA_H
#include<qwt_raster_data.h>
#include<qwt_matrix_raster_data.h>

class WaterfallData: public QwtMatrixRasterData
{
public:
    WaterfallData( double minimumFrequency,
                                  double maximumFrequency);
    void UpdateInterval(double startFreq,double stopFreq);

void addFFTData(double *fftData);
void ResizeData(double startFreq, double stopFreq);

};

#endif // WATERFALLDATA_H
