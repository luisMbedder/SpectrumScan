#include "waterfalldata.h"
#include "Sdrcapture.h"
#include <qwt_matrix_raster_data.h>

WaterfallData::WaterfallData(
                              double minimumFrequency,
                              double maximumFrequency
                               ): QwtMatrixRasterData()//public QwtRasterData
{

           //  this->
          //  int columns = QwtMatrixRasterData::numColumns();
            //int row = this->numRows();
           // QVector<double> matrix = valueMatrix();
        //   double sf=Waterfallplot->GetStartFrequency();
            setInterval( Qt::XAxis, QwtInterval( minimumFrequency, maximumFrequency ) );
            setInterval( Qt::YAxis, QwtInterval( 0, 5 ) );
            setInterval( Qt::ZAxis, QwtInterval( -90.0, -10.0 ) );


        //     setValueMatrix(fftData,1024);
     //       int columns = numColumns();
     //       int row = this->numRows();
    //         QVector<double> matrix = valueMatrix();
           //  qDebug()<<matrix.first();
          //   qDebug()<<matrix.last();
           //  int a=0;
             //replot();


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
    int size = sizeof(result)/sizeof(*result);
    rasterData.assign(result,result+sizeof(result)/sizeof(*result));
    qCopy(result,result+size,rasterVector.begin());


    setValueMatrix(rasterVector,FFT_LENGTH);
   int columns = numColumns();
   int row = this->numRows();
    QVector<double> matrix = valueMatrix();
   //  qDebug()<<matrix.first();
    // qDebug()<<matrix.last();

}
