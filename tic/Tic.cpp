// Copyright (C) Pololu Corporation.  See LICENSE.txt for details.

#include "Tic.h"
#include <string.h>


// ---------------------------------------

class SerialStream: public Stream {
  int fd;
public:
  SerialStream(const char* dev);
  ~SerialStream();
  size_t write(uint8_t byte);
  size_t readBytes(char *buffer, size_t length);
};

#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

SerialStream::SerialStream(const char* dev) {

  fd = open(dev,O_RDWR|O_NOCTTY/*|O_SYNC*/);

  if (fd == -1)
    return;
  
  printf("Open succ\n");

  struct termios settings;
  tcgetattr(fd, &settings);

  cfsetospeed(&settings, B9600); /* baud rate */
  settings.c_cflag &= ~PARENB; /* no parity */
  settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
  settings.c_lflag &= ~ICANON; /* canonical mode */
  settings.c_oflag &= ~OPOST; /* raw output */

  tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
  tcflush(fd, TCOFLUSH);
  printf("flush\n");
}

SerialStream::~SerialStream() {
  if (fd != -1)
    close(fd);
}
size_t SerialStream::write(uint8_t byte) {
  printf("write %x\n",byte);
  size_t ret = ::write(fd,&byte,sizeof(byte));
  printf("written %d\n",ret);
  return ret;
}
size_t SerialStream::readBytes(char *buffer, size_t length) {
  printf("read request len = %d\n",length);  
  size_t ret = ::read(fd, buffer, length);
//  printf("read %d bytes\n",ret);
  return ret;
}

// ---------------------------------------

// ---------------------------------------
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

class BluetoothStream: public Stream {
  
  int sk;

public:
  BluetoothStream(const char* btaddr);
  ~BluetoothStream();

  size_t write(uint8_t byte);
  size_t readBytes(char *buffer, size_t length);
};


BluetoothStream::BluetoothStream(const char* btaddr) {

  struct sockaddr_rc laddr, raddr;

  laddr.rc_family = AF_BLUETOOTH;
  laddr.rc_channel = 0;
  str2ba("00:00:00:00:00:00",&laddr.rc_bdaddr);
  //bacpy(&laddr.rc_bdaddr,BDADDR_ANY);

  raddr.rc_family = AF_BLUETOOTH;
  raddr.rc_channel = 1;
  str2ba(btaddr,&raddr.rc_bdaddr);

  sk = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (sk < 0) {
    perror("Can't create RFCOMM socket");
    return;
  }

  if (bind(sk, (struct sockaddr *) &laddr, sizeof(laddr)) < 0) {
    perror("Can't bind RFCOMM socket");
    close(sk);
    return;
  }

  if (connect(sk, (struct sockaddr *) &raddr, sizeof(raddr)) < 0) {
    perror("Can't connect RFCOMM socket");
    close(sk);
    return;
  }
}

BluetoothStream::~BluetoothStream() {
  if (sk >= 0)
    close(sk);
}
size_t BluetoothStream::write(uint8_t byte) {
//  printf("write %x to sock %d\n",byte,sk);
  size_t ret = ::write(sk,&byte,sizeof(byte));
//  printf("written %d\n",ret);
  return ret;
}
size_t BluetoothStream::readBytes(char *buffer, size_t length) {
//  printf("read request len = %d\n",length);  
  size_t ret = ::read(sk, buffer, length);
//  printf("read %d bytes\n",ret);
  return ret;
}

// ---------------------------------------
#include <stdio.h>
int main() {

//  TicUsb tusb;
//  uint16_t cl = tusb.getCurrentLimit();
//  printf("%d",cl);

  //SerialStream ss("/dev/rfcomm0");
  BluetoothStream ss("00:15:A6:00:43:14");
  TicSerial ts(ss);

  uint16_t cl = ts.getCurrentLimit();
  int step = (int)ts.getStepMode();
  uint32_t uptime = ts.getUpTime();
  uint16_t vinvolt = ts.getVinVoltage();
  int32_t pos = ts.getCurrentPosition();

  printf("Current limit %d\n",cl);
  printf("Step mode %d\n",step);
  printf("Uptime %d\n",uptime);
  printf("Vin voltage %d\n",vinvolt);
  printf("Current position %d\n",pos);

	return 0;
}

// ---------------------------------------

static const uint16_t Tic03aCurrentTable[33] =
{
  0,
  1,
  174,
  343,
  495,
  634,
  762,
  880,
  990,
  1092,
  1189,
  1281,
  1368,
  1452,
  1532,
  1611,
  1687,
  1762,
  1835,
  1909,
  1982,
  2056,
  2131,
  2207,
  2285,
  2366,
  2451,
  2540,
  2634,
  2734,
  2843,
  2962,
  3093,
};

void TicBase::setCurrentLimit(uint16_t limit)
{
  uint8_t code = 0;

  if (product == TicProduct::T500)
  {
    for (uint8_t i = 0; i < 33; i++)
    {
      if (Tic03aCurrentTable[i] <= limit)
      {
        code = i;
      }
      else
      {
        break;
      }
    }
  }
  else if (product == TicProduct::T249)
  {
    code = limit / TicT249CurrentUnits;
  }
  else
  {
    code = limit / TicCurrentUnits;
  }

  commandW7(TicCommand::SetCurrentLimit, code);
}

uint16_t TicBase::getCurrentLimit()
{
  uint8_t code = getVar8(VarOffset::CurrentLimit);
  if (product == TicProduct::T500)
  {
    if (code > 32) { code = 32; }
    return Tic03aCurrentTable[code];
  }
  else if (product == TicProduct::T249)
  {
    return code * TicT249CurrentUnits;
  }
  else
  {
    return code * TicCurrentUnits;
  }
}

/**** TicSerial ****/

void TicSerial::commandW32(TicCommand cmd, uint32_t val)
{
  sendCommandHeader(cmd);

  // byte with MSbs:
  // bit 0 = MSb of first (least significant) data byte
  // bit 1 = MSb of second data byte
  // bit 2 = MSb of third data byte
  // bit 3 = MSb of fourth (most significant) data byte
  serialW7(((val >>  7) & 1) |
           ((val >> 14) & 2) |
           ((val >> 21) & 4) |
           ((val >> 28) & 8));

  serialW7(val >> 0); // least significant byte with MSb cleared
  serialW7(val >> 8);
  serialW7(val >> 16);
  serialW7(val >> 24); // most significant byte with MSb cleared

  _lastError = 0;
}

void TicSerial::commandW7(TicCommand cmd, uint8_t val)
{
  sendCommandHeader(cmd);
  serialW7(val);

  _lastError = 0;
}

void TicSerial::getSegment(TicCommand cmd, uint8_t offset,
  uint8_t length, void * buffer)
{
  length &= 0x7F;
  sendCommandHeader(cmd);
  serialW7(offset);
  serialW7(length);

  uint8_t byteCount = _stream->readBytes((uint8_t *)buffer, length);
  if (byteCount != length)
  {
    _lastError = 50;

    // Set the buffer bytes to 0 so the program will not use an uninitialized
    // variable.
    memset(buffer, 0, length);
    return;
  }

  _lastError = 0;
}

void TicSerial::sendCommandHeader(TicCommand cmd)
{
  if (_deviceNumber == 255)
  {
    // Compact protocol
    _stream->write((uint8_t)cmd);
  }
  else
  {
    // Pololu protocol
    _stream->write(0xAA);
    serialW7(_deviceNumber);
    serialW7((uint8_t)cmd);
  }
  _lastError = 0;
}

