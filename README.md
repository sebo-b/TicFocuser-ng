# TicFocuser
INDI Driver for USB focuser (TIC)

With the TIC Focuser, the Pololu TIC controllers (like Tic T825) can be addressed via the KStars - Ekos platform via USB. 

This project was inspired by Radek Kaczorek's "astroberry-diy" drivers, designed for the Raspberry Pi and similar single board computers. See his repository for reference: https://github.com/rkaczorek/astroberry-diy

Strictly required is the implementation of the TIC software, see following link:

https://github.com/pololu/pololu-tic-software

Please also take note of the instructions provided in the Userguide:
https://www.pololu.com/docs/0J71/3.2

Also to be observed: The TIC has a timeout feature, unless disabled, will stopp the motor after about 1 second. This is also explained in the above guide, see chapter 12.1. To overcome: Open the TIC control center (ticgui) and uncheck "enable command time  out". I have added the ticgui in the autostart folder (Ubuntu 18.04.)

1) $ git clone https://github.com/HelgeMK/TicFocuser.git
2) $ cd TicFocuser
3) $ mkdir build && cd build
4) $ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
5) $ make
6) $ sudo make install

To use this driver, open in KStars the profile editor and enter TIC Focuser for the focuser driver.

I have written and tested this program using the Tic T825 controller. For my nema17 stepper with gearbox, I have also updated the current limit in the a.m. TIC control center. Please be careful and also read related section in the Userguide, before touching.

Disclaimer:

All of this has been working fine for me. I am neither a professional programmer nor an electrician and cannot tell wether it works in general, or in worst case could cause any damage to you or your equipment. Using this repository is at your own risk.
