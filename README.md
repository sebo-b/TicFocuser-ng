# TicFocuser-ng
INDI Driver for USB focuser based on Pololu Tic controller.

This project was initially a fork of TicFocuser driver written by Helge Kutzop and published on [GitHub](https://github.com/HelgeMK/TicFocuser) as it seemed that the author is not maintaining it anymore. However, the refactoring of code was such deep, that I'm not sure if a single line of the original code stayed in this version. Helge was basing his version on Radek Kaczorek's "astroberry-diy" drivers published on [GitHub](https://github.com/rkaczorek/astroberry-diy). As said I'm quite certain that all old code was rewritten, nevertheless to respect both authors I kept their names in AUTHORS file.

# Whys

You may wonder why we have created another focuser as there are many already available on the market. Here are my reasons for it:
* DIY focuser is much cheaper than a manufactured one.
* DIY is more fun.
* I wanted something small and simple for astrophotography, without an external controller.
* I wanted a focuser which can be connected directly to a computer (PC, Laptop or RaspberryPi), so USB seemed to be the best choice.
* I wanted something aesthetic.

Pololu Tic controller fulfills all these needs. It is relatively cheap, small and among others, it supports USB (no need for GPIOs or extra Arduino).

# Software

This is a driver for a great framework: INDI. INDI together with KStars is opensource, works natively on Linux and MacOS, and can fully control astrophotography sessions. I was using it on RaspberryPi 3B+ and later on, I switched to much more powerful Intel NUC mini-pc. 

### Connections

The driver can utilize various connection interfaces to communicate with Pololu Tic controller. Some connections require particular libraries to be installed in your machine, CMake script will automatically detect these dependencies and enable only these connections, which can be compiled.

Available connections:
1. PololuUSB - this is USB connection which utilizes (Pololu Tic C library)[https://github.com/pololu/pololu-tic-software]. Check (dependencies)[#Dependencies] section for more info about installing it.
2. LibUSB - this is USB connection with utilizes standard libusb-1 probably available on all Linux distributiins. It also modified [Pololu Tic Arduino library](https://github.com/pololu/tic-arduino). Only libusb-1 is needed to make it work.
3. Bluetooth - this is serial Bluetooth connection. Uses [Pololu Tic Arduino library](https://github.com/pololu/tic-arduino) and depends on standard libbluetooth.
4. Serial - this is regular serial connection (after configuring rfcomm tty can be also used for Bluetooth). This connection is always compiled.

### Dependencies

1. You need INDI library installed together with headers. Most of Linux distributions have it in the repo. To install it on Arch simply type `pacman -S libindi`

2. PololuUSB - you need Pololu Tic library. You can find instructions on how to download it and compile on [Pololu site](https://www.pololu.com/docs/0J71). On Arch you can use AUR package created by myself available [here](https://aur.archlinux.org/packages/pololu-tic-software/).

3. LibUSB - you need libusb-1. If it is not installed, install it from your Linux distribution repository.

4. LibBluetooth - you need libbluetooth. If it is not installed, install it from your Linux distribution repository.

### Compilation

In general, you should use released version instead of git bleeding-edge work. The latest release was v0.9.

```
$ curl -L https://github.com/sebo-b/TicFocuser-ng/archive/v0.9.tar.gz | tar -xz
$ cd TicFocuser-ng-0.9
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
$ sudo make install
```
To automatically create Arch package you can use AUR package created by myself available [here](https://aur.archlinux.org/packages/libindi-ticfocuser/).

### Configure Pololu Tic

If you haven't done so, execute `ticgui` and properly configure Tic controller. For various reasons, motor parameters are not controlled by TicFocuser driver.

At minimum:
* Configure your motor parameters like: current limit, step mode, speed, acceleration, etc.

![ticcmd](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/ticgui_motorsettings.png)

* Uncheck `Enable command timeout` checkbox in the `Serial` box. More info abt it in [chapter 4.4 of Pololu Tic documentation](https://www.pololu.com/docs/0J71/all#4.4). 

* Test from `ticgui` if your focuser works correctly.

You can read more about setting up proper motor parameters in [chapter 4.3 of Pololu Tic documentation](https://www.pololu.com/docs/0J71/all#4.3). In my setup I'm using current limit much lower than the rate of my motor because not much force is needed for my focuser, I save power and what's most important vibrations caused by the motor are marginalized.

### Configure KStars

Add `TicFocuser` to INDI profile in KStars and you are good to go.

# Hardware

### What will you need
1. Tic Motor controller (without gold pins). I used T825 which looks like that:

![T825](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/tic825.jpg)

2. NEMA-17 stepper motor. I'm using an equivalent of [Pololu #2267](https://www.pololu.com/product/2267).

![NEMA17](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/nema17.jpg)

3. Possibility to 3D print the case.
4. DC Barrel Power Jack Socket 5.5mm/2.1mm.

![DC](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/dc_barrel.jpg)

5. Basic soldering skills.
6. A way to connect the stepper motor to your focuser.

### 3D printed case

Model of controller case looks like the following:

![Case](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/case1.jpg)

Case after printout mounted on the motor:

![Case](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/case2.jpg)

With all elements inside:

![Case](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/case3.jpg)

And fully assembled and mounted on the tube:

![Case](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/focuser_final.jpg)

A project of the case is [attached as STL file](https://raw.githubusercontent.com/sebo-b/TicFocuser-ng/master/extras/ticfocuser_case.stl) for 3D printer. Please note that this file is in 10:1 scale, so it needs to be 10x rescaled down before printing. 

Enjoy!

# Disclaimer

This software is working fine for me on Arch Linux with INDI v1.7.9 on x86 and Tic T825. I haven't tested it on RaspberryPi as I switched from this platform. This software is not changing any electrical parameters of Tic controller configuration. However, each software has bugs. I don't take any responsibility for it and you are using it at *your own risk*.
