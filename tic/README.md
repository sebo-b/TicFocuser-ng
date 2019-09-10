
This is a modified [tic-arduino](https://github.com/pololu/tic-arduino) library.

Goals:
- make it work on Linux,
- make as few changes as possible (to efficiently follow the origin).

Reasons to use tic-arduino over (tic-usb library)[https://github.com/pololu/pololu-tic-software]:
- it is more generic than tic-usb library, which supports only USB connection,
- it can be easily extended to support Serial, Bluetooth or I2C,
- it doesn't depend on Pololu (libusbp library)[https://github.com/pololu/libusbp],
- it is light and comes without external dependencies (except regular libusb for USB connection).

All changes of code are published on the same license as the original library, which is MIT. 
