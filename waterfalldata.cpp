#include "waterfalldata.h"
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

void WaterfallData::addFFTData(double *fftData){

    std::vector<double> rasterData;
    QVector<double> rasterVector(1024);
    int size = sizeof(fftData)/sizeof(*fftData);
    rasterData.assign(fftData,fftData+sizeof(fftData)/sizeof(*fftData));
    qCopy(fftData,fftData+size,rasterVector.begin());


    setValueMatrix(rasterVector,1024);
   int columns = numColumns();
   int row = this->numRows();
    QVector<double> matrix = valueMatrix();

}
