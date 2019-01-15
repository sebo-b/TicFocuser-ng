# TicFocuser
INDI Driver for USB focuser (Tic)

With the Tic Focuser driver, stepper motors can be controlled thru the Pololu Tic controller (like Tic T825) via the KStars - Ekos platform directly using the USB interface. This can simplify the set-up, as no longer an ARDUINO or similar microcontrollers are needed in-between the stepper controller and the INDI server. 

This project was inspired by Radek Kaczorek's "astroberry-diy" drivers, designed for the Raspberry Pi and similar single board computers. See his repository for reference: https://github.com/rkaczorek/astroberry-diy

First, please install the INDI Library Repository:

https://github.com/indilib/indi

Strictly required is the implementation of the Tic software, see following link:

https://github.com/pololu/pololu-tic-software

Please also take note of the instructions provided in the Pololu Tic Userguide:
https://www.pololu.com/docs/0J71/3.2

Also to be observed: The Pololu Tic has a timeout feature, unless disabled, will stopp the motor after about 1 second. This is also explained in the above guide, see chapter 12.1. To overcome: Open the Tic control center (ticgui) and uncheck "enable command time  out". I have added the ticgui in the autostart folder (Ubuntu 18.04.)

1) $ git clone https://github.com/HelgeMK/TicFocuser.git
2) $ cd TicFocuser
3) $ mkdir build && cd build
4) $ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
5) $ make
6) $ sudo make install

To use this driver, open in KStars the profile editor and enter Tic Focuser for the focuser driver.

I have written and tested this program using the Pololu Tic T825 controller. For my nema17 stepper with gearbox, I have also updated the current limit in the a.m. Tic control center. Please be careful and also read related section in the Userguide, before touching.

Disclaimer:

All of this has been working fine for me. I am neither a professional programmer nor an electrician and cannot tell whether it works in general, or in worst case could cause any damage to you or your equipment. Using this repository is at your own risk.
