/********************************************************************
* Project: SpectrumScan
*
* File: gui.cpp
*
* Created by: LuisMbedder
*
* Description: This class creates the instantanous and waterfall
*               plots, initializes the SDR hardware,handles the
*               GUI events, and processes the IQ data samples.
*
* notes:
********************************************************************/

#include "gui.h"
#include "ui_gui.h"
#include "spectrograph.h"
#include "waterfallplot.h"
#include <QtCore>
#include <QtGui>
#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include "Sdrcapture.h"
#include <QVector>
#include "math.h"
#include <stdio.h>

Gui::Gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gui)
{

    count = 0;
    display_locked_frequency = 0;

    overflow_warning_displayed = false;
    save_next = false;
    mode = SPECTROGRAPH;
    //create spectrograph plot, default plot
    plot = new Spectrograph(this);
    plot->setGeometry(10, 10, 780, 330);

    QwtScaleWidget *bottomScale =plot->axisWidget(QwtPlot::xBottom);
    QPalette bottomPalette = bottomScale->palette();
    bottomPalette.setColor( QPalette::WindowText, Qt::white); // for ticks
    bottomPalette.setColor( QPalette::Text, Qt::white); //for ticks labels
    bottomScale->setPalette( bottomPalette );

    QwtScaleWidget *leftScale = plot->axisWidget(QwtPlot::yLeft);
    QPalette leftPalette = leftScale->palette();
    leftPalette.setColor( QPalette::WindowText, Qt::white); // for ticks
    leftPalette.setColor( QPalette::Text, Qt::white); // for ticks labels
    leftScale->setPalette( leftPalette );

    //create waterfall plot, hidden by default
    waterfall = new Waterfallplot(this);
    waterfall->setGeometry(10,10,780,330);
    waterfall->hide();

    ui->setupUi(this);
    ui->label_gainhigh->setVisible(false);

#ifdef USE_AVFFT
    fft_context = av_fft_init(FFT_LENGTH_2N,0);//set up a complex FFT
    fft_data = (FFTComplex*) av_malloc(FFT_LENGTH * sizeof(FFTComplex));
#endif

#ifdef BLACKMANNUTALL_WINDOW
    float a0 = 0.3635819;
    float a1 = 0.4891775;
    float a2 = 0.1365995;
    float a3 = 0.0106411;

    for(int i=0; i<FFT_LENGTH; ++i){
        window[i] = a0 - a1 * cos(2.*M_PI*i/(FFT_LENGTH-1)) + a2 * cos(4.*M_PI*i/(FFT_LENGTH-1)) - a3 * cos(6.*M_PI*i/(FFT_LENGTH-1));
    }
#endif

#ifdef HAMMING_WINDOW
    float alpha = 0.54;
    float beta = 0.46;

    for(int i=0; i<FFT_LENGTH; ++i){
        window[i] = alpha - beta * cos(2*M_PI*i/(FFT_LENGTH-1));
    }
#endif

    dc_i_average = 127;
    dc_q_average = 127;

    gain_correction = 0;
    phase_correction = 0;

    sdrCapture = new Sdrcapture();//initialize SDR hardware
    num_gains = sdrCapture->get_num_gains();
    ui->gainSlider->setMaximum(num_gains-1);
    sdrThread = new QThread();
    sdrCapture->moveToThread(sdrThread);

    // Connects slots
    sdrCapture->connect(this, SIGNAL(startCapture()),SLOT(threadFunction()));
    this->connect(sdrCapture, SIGNAL(packetCaptured()), SLOT(doneCapture()));
    sdrCapture->connect(this, SIGNAL(setRtlFrequency(unsigned)), SLOT(set_frequency(unsigned)));
    sdrCapture->connect(this, SIGNAL(setRtlGain(unsigned)), SLOT(set_gain(unsigned)));
    sdrCapture->connect(this, SIGNAL(setRtlOffsetTuning(bool)), SLOT(set_offset_tuning(bool)));
    sdr_buffer = sdrCapture->getBuffer();
    sdrThread->start();
    set_target_frequency(DEFAULT_CENTER_FREQUENCY);
    set_target_gain(1);
    emit startCapture();
    set_target_gain(num_gains/2);
}

Gui::~Gui()
{
    delete ui;
}

/*****************************************************************************
 * This method is called after a block of IQ data has been captured
 * The Power Spectral Density(PSD) is calculated using a 50% overlap to
 * to compensate for the attenuation at the ends of the frames
 * caused by applying the window function.
 * The collection of another block of IQ data is triggered at the end of
 * the method.
 ****************************************************************************/
void Gui::doneCapture()
{
    count++;             //Counter for working out FFT update rate
    QString text;
    text.sprintf("%d", count);

    uint8_t re,im;
    float f_re, f_im;
    float ptr, pti;
    bool overflow = false;

    int i,j;
    for (i = 0; i < FFT_LENGTH; i++) {
        data_result[i] = 0;         //Initialize PSD output
        re = sdr_buffer[2*i];       //Check for overflow
        im = sdr_buffer[2*i+1];
        if(re<5 || re>250 || im<5 || im>250) overflow = true;
    }

    //calculate block DC average
    ptr = pti = 0;
    for (i=0; i<BLOCK_LENGTH; ++i){
        ptr += sdr_buffer[2*i];
        pti += sdr_buffer[2*i+1];
    }
    dc_i_average += 0.1f*((ptr/BLOCK_LENGTH)-dc_i_average);
    dc_q_average += 0.1f*((pti/BLOCK_LENGTH)-dc_q_average);

    //Calculate PSD
    int slide_length = 0;
    for(j=0; j< FFT_NUM_BLOCKS; ++j){
        for (i = 0; i < FFT_LENGTH; i++) {
            //remove the DC average from the signal before applying the FFT
            f_re = sdr_buffer[slide_length+2*i] - dc_i_average;
            f_im = sdr_buffer[slide_length+2*i+1] - dc_q_average;
            //apply hamming window
            ptr = (f_re) * (1.0f + gain_correction)* window[i];
            pti = (f_im + f_re*phase_correction) * window[i];

#ifdef USE_AVFFT
            fft_data[i].re = ptr;
            fft_data[i].im = pti;
#endif
#ifdef USE_KISSFFT
            fin[i].r = ptr;
            fin[i].i = pti;
#endif
        }

#ifdef USE_AVFFT
        //calculate FFT
        av_fft_permute(fft_context, fft_data);
        av_fft_calc(fft_context, fft_data);
#endif
#ifdef USE_KISSFFT
        kiss_fft(mycfg, fin, fout);
#endif

        for (i = 0; i < FFT_LENGTH; i++) {
#ifdef USE_AVFFT
            //calculate power spectrun => [sqrt(re^2+im^2)]^2
            data_result[i] += fft_data[i].re*fft_data[i].re+fft_data[i].im*fft_data[i].im;
#endif
#ifdef USE_KISSFFT
            data_result[i] += fout[i].r*fout[i].r+fout[i].i*fout[i].i;
#endif
        }
        slide_length += FFT_SLIDE;
    }

    //Convert to dB
    for (i = 0; i < FFT_LENGTH; i++) {
        data_result[i] = 10. * log10(data_result[i]/FFT_NUM_BLOCKS)-100;
    }


    //Display overflow - gain too high warning
    if(overflow != overflow_warning_displayed){
        if(overflow){
            ui->label_gainhigh->setVisible(true);
        }
        else{
            ui->label_gainhigh->setVisible(false);
        }
        overflow_warning_displayed = overflow;
    }

    //Update graph display range and title if frequency changes
    QString title;
    QwtText title2;
    if(sdrCapture->is_locked){
         if(display_locked_frequency!= sdrCapture->locked_frequency){
             double freq_MHz = sdrCapture->locked_frequency/1000000.0;
             title.sprintf("Center %4.1f MHz", freq_MHz);
             title2=(QwtText)title;
             title2.setColor(Qt::white);
             plot->setTitle(title2);
             waterfall->setTitle(title2);
             //bandwidth is 2.4MHz so +-(2.4MHz/2) to center freq_MHz
             plot->SetXRange(freq_MHz-1.2, freq_MHz+1.2);
             waterfall->SetFrequencyRange(freq_MHz-1.2, freq_MHz+1.2);
             display_locked_frequency = sdrCapture->locked_frequency;
         }
    }
    else{
        title.sprintf("Frequency Not Locked [%4.1f MHz]", target_frequency/1000000.);
        title2=(QwtText)title;
        title2.setColor(Qt::white);
        plot->setTitle(title2);
        plot->setTitle(title);
    }

    plot->SetData(data_result);
    waterfall->PlotNewData(data_result);

    //Save IQ and FFT block if requested
    if(save_next){
        FILE* fp = fopen("sample_dump.txt", "w");
        for(int i=0; i<BLOCK_LENGTH; ++i){
            fprintf(fp, "%d %d\n", sdr_buffer[2*i], sdr_buffer[2*i+1]);
        }
        fclose(fp);

        fp = fopen("fft_dump.txt", "w");
        for(int i=0; i<FFT_LENGTH; ++i){
#ifdef USE_KISSFFT
            ptr = fout[i].r;
            pti = fout[i].i;
#endif
#ifdef USE_AVFFT
            ptr = fft_data[i].re;
            pti = fft_data[i].im;
#endif
            fprintf(fp, "%f + %fi\n", ptr, pti);

        }
        fclose(fp);
        save_next = false;
    }

    //Change frequency if +0.1MHz or -0.1MHz button is pressed
    if(ui->pushButton_Inc01->isDown()){
        update_target_frequency(100000);
    }
    if(ui->pushButton_Dec01->isDown()){
        update_target_frequency(-100000);
    }

    emit startCapture();
}


/*****************************************************************************
 * Set target frequency
 ****************************************************************************/
void Gui::set_target_frequency(u_int32_t frequency)
{
    target_frequency = frequency;
    QString text;
    text.sprintf("Target: %4.1f MHz", target_frequency/1000000.);
    emit setRtlFrequency(frequency);
}

/*****************************************************************************
 * Change target frequency by the amount delta
 ****************************************************************************/
void Gui::update_target_frequency(int32_t delta)
{
    u_int32_t new_frequency = target_frequency + delta;
    if((20000000u<new_frequency) && (new_frequency<2210000000u)){
        set_target_frequency(new_frequency);
    }
}

/*****************************************************************************
 * Sets LNA gain
 ****************************************************************************/
void Gui::set_target_gain(unsigned gain_index)
{
    target_gain = gain_index;
    if(target_gain>num_gains-1){
        target_gain = num_gains-1;
    }
    ui->gainSlider->setValue(target_gain);
    emit setRtlGain(target_gain);
}


/*****************************************************************************
 * Event handlers for changing frequency.
 ****************************************************************************/
void Gui::on_pushButton_Inc100_clicked()
{
    update_target_frequency(100000000);
}

void Gui::on_pushButton_Dec100_clicked()
{
    update_target_frequency(-100000000);
}

void Gui::on_pushButton_Inc10_clicked()
{
    update_target_frequency(10000000);
}

void Gui::on_pushButton_Dec10_clicked()
{
    update_target_frequency(-10000000);
}

void Gui::on_pushButton_Inc1_clicked()
{
    update_target_frequency(1000000);
}

void Gui::on_pushButton_Dec1_clicked()
{
    update_target_frequency(-1000000);
}

void Gui::on_pushButton_Inc01_clicked()
{
    update_target_frequency(100000);
}

void Gui::on_pushButton_Dec01_clicked()
{
    update_target_frequency(-100000);
}

/*****************************************************************************
 * Event handlers for increasing or decreasing gain
 ****************************************************************************/
void Gui::on_pushButton_GainInc_2_clicked()
{
    set_target_gain(target_gain+1);
}

void Gui::on_pushButton_GainDec_clicked()
{
    if(target_gain>0){
        set_target_gain(target_gain-1);
    }
}


/*****************************************************************************
 * Exit
 ****************************************************************************/
void Gui::on_pushButton_Exit_clicked()
{
    qApp->exit();

}

/*****************************************************************************
 * Switch plot mode
 ****************************************************************************/
void Gui::on_pushButton_Mode_clicked()
{

  if(mode==SPECTROGRAPH){
        mode=WATERFALL;
        plot->hide();
        waterfall->show();
    }
    else{

        mode=SPECTROGRAPH;
        waterfall->hide();
        plot->show();
    }

}

// End of Gui.cpp
