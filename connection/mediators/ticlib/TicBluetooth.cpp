/*******************************************************************************
TicFocuser
Copyright (C) 2019 Sebastian Baberowski

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

#include "TicBluetooth.h"

#include <string.h>

enum TicBluetoothError {
  NO_ERROR = 0,
  FAILED_TO_CONNECT = -1,
};

static const char* ticBluetoothErrorMsg[] = 
{
  "No error",
  "Failed to connect",
};

TicBluetooth::TicBluetooth(uint8_t deviceNumber):
  TicSerial(stream, deviceNumber)
{
  _lastError = NO_ERROR;
}

TicBluetooth::~TicBluetooth() 
{
  disconnect();
}

void TicBluetooth::connect(const char* serialNo)
{
  _lastError = stream.connect(serialNo)? NO_ERROR: FAILED_TO_CONNECT;
}

void TicBluetooth::disconnect()
{
  stream.disconnect();
}

const char* TicBluetooth::getLastErrorMsg()
{
  if (_lastError > 0 || -(int)_lastError <= sizeof(ticBluetoothErrorMsg)/sizeof(ticBluetoothErrorMsg[0]) )
    return "UNKNOWN";

  return ticBluetoothErrorMsg[ -(int)_lastError ];
}
