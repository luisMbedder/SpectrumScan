#ifndef GUI_H
#define GUI_H

#include <QDialog>
#include "spectrograph.h"
#include "Sdrcapture.h"
#include "waterfallplot.h"

#ifdef USE_AVFFT
extern "C" {
#include "libavcodec/avfft.h"
#include "libavutil/mem.h"
}
#endif

namespace Ui {
class Gui;
}

class Gui : public QDialog
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = 0);
    ~Gui();

private slots:

    void doneCapture();
    void on_pushButton_Inc100_clicked();
    void on_pushButton_Dec100_clicked();
    void on_pushButton_Inc10_clicked();
    void on_pushButton_Dec10_clicked();
    void on_pushButton_Inc1_clicked();
    void on_pushButton_Dec1_clicked();
    void on_pushButton_Inc01_clicked();
    void on_pushButton_Dec01_clicked();
    void on_pushButton_GainInc_2_clicked();
    void on_pushButton_GainDec_clicked();
    void on_checkBox_offsettuning_clicked(bool checked);
    void on_pushButton_Capture_clicked();
    void on_pushButton_Exit_clicked();

    void on_pushButton_Mode_clicked();

private:
    Ui::Gui *ui;

    QTimer *timer;

    Sdrcapture *sdrCapture;
    QThread *sdrThread;
    Spectrograph *plot;
    Waterfallplot *waterfall;
    uint8_t* sdr_buffer;
    int count;

    float window[FFT_LENGTH];

    double data_result[RESULT_LENGTH];

    int data_counter;
    u_int32_t target_frequency;
    unsigned target_gain;

    void set_target_frequency(u_int32_t frequency);
    void update_target_frequency(int32_t delta);

    void set_target_gain(unsigned gain_index);
    u_int32_t display_locked_frequency;
    bool display_is_locked;

    bool overflow_warning_displayed;

    bool save_next;
    enum GRAPH_MODE_T
    {
       SPECTROGRAPH,
       WATERFALL
    };

    GRAPH_MODE_T mode;


    float dc_i_average;
    float dc_q_average;

    float gain_correction;
    float phase_correction;

    unsigned num_gains;

#ifdef USE_AVFFT
    FFTContext* fft_context;
    FFTComplex* fft_data;
#endif

signals:
    void startCapture();
    void setRtlFrequency(unsigned freq);
    void setRtlGain(unsigned gain_index);
    void setRtlOffsetTuning(bool on);



};

#endif // GUI_H
