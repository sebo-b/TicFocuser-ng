# TicFocuser v2
INDI Driver for USB focuser based on Pololu Tic controller.

This project was initially a fork of TicFocuser driver written by Helge Kutzop and published on [GitHub][https://github.com/HelgeMK/TicFocuser] as it seemed that the author is not maintaining it anymore. However, the refactor of code was such deep, that I'm not sure if a single line of the original code stayed in this version. Helge was basing his version on Radek Kaczorek's "astroberry-diy" drivers published on [GitHub][https://github.com/rkaczorek/astroberry-diy]. As said I'm quite certain that all old code was rewritten, nevertheless to respect both authors I kept their names in AUTHORS file.

# Whys

You may wonder why we have created another focuser as there are many already available on the market. Here are my reasons for it:
* DIY focuser is much cheaper than a manufactured one.
* DIY is more fun.
* I wanted something small and simple for astrophotography, without an external controller.
* I wanted a focuser which can be connected directly to a computer (PC, Laptop or RaspberryPi), so USB seemed to be the best choice.
* I wanted something aesthetic.

Pololu Tic controller fulfills all these needs, it is relatively cheap, small and among others, it supports USB (no need for GPIOs or extra Arduino).

# Software

This is a driver for a great framework: INDI. INDI together with KStars is opensource, works natively on Linux and MacOS, and can fully control astrophotography sessions. I was using it on RaspberryPi 3B+ and later on, I switched to much more powerful Intel NUC mini-pc. 

### Dependencies

1. You need INDI library installed together with headers. Most of Linux distributions have it in the repo. To install it on Arch simply type `pacman -S libindi`

2. You need Pololu Tic library. You can find instructions how to download it and compile on [Pololu site][https://www.pololu.com/docs/0J71]. On Arch you can use AUR package created by myself available [here][https://aur.archlinux.org/packages/pololu-tic-software/].

### Compilation

```
$ git clone git@github.com:sebo-b/TicFocuser.git
$ cd TicFocuser
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
$ sudo make install
```
To automatically create Arch package you can use AUR package created by myself available [here][https://aur.archlinux.org/packages/libindi-ticfocuser/].

### Configure Pololu Tic

If you haven't do so, execute `ticgui` and properly configure Tic controller. 
At minimum:
* Select current limit for you step motor.
* Select speed and acceleration.
* Uncheck `Enable command timeout` checkbox in the `Serial` box. More info abt in in [Pololu Documentation in chapter 4.4][https://www.pololu.com/docs/0J71/all#4.4]. 

These parameters are not controlled by this driver.

### Configure KStars

Add `TicFocuser` to INDI profile in KStars and you are good to go.

# Hardware

### What will you need
1. Tic Motor controller (without gold pins). I used T825 which looks like that:

![T825](https://raw.githubusercontent.com/sebo-b/TicFocuser/refactor/extras/tic825.jpg)

2. NEMA-17 stepper motor. I'm using an equivalent of [Pololu #2267][https://www.pololu.com/product/2267].

![NEMA17](https://raw.githubusercontent.com/sebo-b/TicFocuser/refactor/extras/nama17.jpg)

3. Possibility to 3D print the case.
4. DC Barrel Power Jack Socket 5.5mm/2.1mm.

![DC](https://raw.githubusercontent.com/sebo-b/TicFocuser/refactor/extras/dc_barrel.jpg)

5. Basic ironing skills.
6. A way to connect the stepper motor to your focuser.

# Disclaimer

This software is working fine for me on Arch Linux with INDI v1.7.9 on x86 and Tic T825. I haven't tested it on RaspberryPi as I switched from this platform. This software is not changing any electrical parameters of Tic controller configuration, however each software has bugs. I don't take any responsibility for it and you are using it at *your own risk*.
