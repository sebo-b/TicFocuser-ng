/*******************************************************************************
TicFocuser
Copyright (C) 2019 Sebastian BaberowbtSocketi

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*******************************************************************************/

#include "StreamBT.h"

#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

StreamBT::StreamBT():
	btSocket(-1)
{
}

StreamBT::~StreamBT()
{
	disconnect();
}

bool StreamBT::connect(const char* btMacAddress)
{
  struct sockaddr_rc laddr, raddr;

  laddr.rc_family = AF_BLUETOOTH;
  laddr.rc_channel = 0;
  str2ba("00:00:00:00:00:00",&laddr.rc_bdaddr);
  //bacpy(&laddr.rc_bdaddr,BDADDR_ANY);

  raddr.rc_family = AF_BLUETOOTH;
  raddr.rc_channel = 1;
  str2ba(btMacAddress,&raddr.rc_bdaddr);

  btSocket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (btSocket < 0)
  	return false;

  if (::bind(btSocket, (struct sockaddr *) &laddr, sizeof(laddr)) < 0) {
    close(btSocket);
    return false;
  }

  if (::connect(btSocket, (struct sockaddr *) &raddr, sizeof(raddr)) < 0) {
    close(btSocket);
    return false;
  }

  return true;
}

void StreamBT::disconnect()
{
	if (btSocket >= 0)
		close(btSocket);
	btSocket = -1;
}

size_t StreamBT::write(uint8_t byte)
{
  return ::write(btSocket,&byte,sizeof(byte));
}

size_t StreamBT::readBytes(char *buffer, size_t length)
{
  size_t readC = 0;
  int numZeros = 0; // safety counter, if we read 5x0 in a row, we eject

  // try to read until we receive enough bytes
  while (readC < length && numZeros < 5) {

    if (readC > 0)
      usleep(10);
    
    size_t c = ::read(btSocket, buffer + readC, length - readC);
    readC += c;

    if (c == 0)
      ++numZeros;
    else
      numZeros = 0;
  }

  return readC;
}

