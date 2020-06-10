# DFPlayerMini 
 
A C++ library to use simple functionalities of the DFPlayerMini without the need to send any bytecodes or calculate any checksums.
It can be used combined with a FTDI (USB-Serial-Converter) via USB or directly with the UART pins of a Raspberry Pi. 
 
Hardware setup:  
&nbsp;&nbsp;The DFPlayer is connected via UART with 3 wires (RX<->TX, TX<->RX, GND<->GND).   
&nbsp;&nbsp;It is advised to put a 1k resistor between the TX of your UART interface and the RX of the DFPlayerMini. 
