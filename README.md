SpectrumScan
============
SpectrumScan is a spectrum analyzer with a 2GHz frequency range built on the beaglebone black that allows you capture and display the radio wave frequencies that we're surrounded by everyday. It works by using a USB DVB-T television reciever based on the RTL2832 demodulator chip which allows direct access to the I/Q data using the rtl-sdr library. A Fast-Fourier-Transform(FFT) is performed on the sampled signals which converts the data to the frequency domain and are then plotted on the screen. 

![Picture](https://cloud.githubusercontent.com/assets/7320156/10060975/521803f0-6212-11e5-9a73-0bb6cc7113f4.png)

Build your own SpectrumScan!: http://luismbedder.com/SpectrumScan/

License
-------
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.
[http://www.gnu.org/licenses/gpl.html](http://www.gnu.org/licenses/gpl.html) GPL version 3 or higher

principal author: Luis Marquez <luismbedder.com>
