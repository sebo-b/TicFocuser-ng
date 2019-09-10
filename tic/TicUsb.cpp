/*******************************************************************************
TicFocuser
Copyright (C) 2019 Sebastian Baberowski

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "TicUsb.h"

#include <libusb.h>
#include <string.h>

// copied from pololu-tic-software/include/tic_protocol.h
// licensed on MIT license
#define TIC_VENDOR_ID 0x1FFB
#define TIC_PRODUCT_ID_T825 0x00B3
#define TIC_PRODUCT_ID_T834 0x00B5
#define TIC_PRODUCT_ID_T500 0x00BD
#define TIC_PRODUCT_ID_N825 0x00C3
#define TIC_PRODUCT_ID_T249 0x00C9

static const size_t MAX_SERIAL_NUMBER = 20; // serial number has 8 characters, 20 is super safe

TicUsb::TicUsb(const char* serialNo):
  handle(NULL), context(NULL)
{

  _lastError = libusb_init(&context);
  if (_lastError)
    return;

  libusb_device** devs = NULL;
  _lastError = libusb_get_device_list(NULL, &devs);
  if (_lastError <= 0)
    return;
  
  _lastError = 0;

  size_t serialNoLen = serialNo? strlen(serialNo): 0;
  libusb_device** dev = devs;

  for (; *dev != NULL; ++dev) 
  {
    struct libusb_device_descriptor desc;
    _lastError = libusb_get_device_descriptor(*dev, &desc);
    if (!_lastError)
      break;

    if (desc.idVendor != TIC_VENDOR_ID ||
        (desc.idProduct != TIC_PRODUCT_ID_T825 &&
         desc.idProduct != TIC_PRODUCT_ID_T825 &&
         desc.idProduct != TIC_PRODUCT_ID_T825 &&
         desc.idProduct != TIC_PRODUCT_ID_T825) )
    {
      continue;
    }

    if (!desc.iSerialNumber)
      continue;

    _lastError = libusb_open(*dev, &handle);
    if (!_lastError) 
    {
      handle = NULL;
      break;
    }

    unsigned char devSerial[MAX_SERIAL_NUMBER];
    size_t serialLen = 0;

    serialLen = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, devSerial, MAX_SERIAL_NUMBER);
    if (serialLen >= 0 &&
        (serialNoLen == 0 || 
          (serialLen == serialNoLen && !strncmp(serialNo,(char*)devSerial,serialLen))) ) 
    {
      break;
    }

    libusb_close(handle);
    handle = NULL;      
    _lastError = serialLen > 0? 0: serialLen;
  }

  libusb_free_device_list(devs,1);

  if (_lastError)
    return;

  if (!handle)
    _lastError = LIBUSB_ERROR_NO_DEVICE;
}

TicUsb::~TicUsb() 
{
  if (handle)
    libusb_close(handle);

  if (context)
    libusb_exit(context);

  context = NULL;
  handle = NULL;
}

void TicUsb::commandQuick(TicCommand cmd)  
{
  _lastError = libusb_control_transfer(handle, 0x40, (uint8_t)cmd, 0, 0, NULL, 0, 0);
  _lastError = _lastError > 0? 0: _lastError;
}
void TicUsb::commandW32(TicCommand cmd, uint32_t val)  {

  uint16_t wValue = (uint32_t)val;
  uint16_t wIndex = (uint32_t)val >> 16;
  
  _lastError = libusb_control_transfer(handle, 0x40, (uint8_t)cmd, wValue, wIndex, NULL, 0, 0);
  _lastError = _lastError > 0? 0: _lastError;
}

void TicUsb::commandW7(TicCommand cmd, uint8_t val)  
{
  _lastError = libusb_control_transfer(handle, 0x40, (uint8_t)cmd, val, 0, NULL, 0, 0);
  _lastError = _lastError > 0? 0: _lastError;
}

void TicUsb::getSegment(TicCommand cmd, uint8_t offset, uint8_t length, void * buffer)  
{
  _lastError = libusb_control_transfer(handle, 0xC0, (uint8_t)cmd, 0, offset, (unsigned char*)buffer, length, 0);

  if (_lastError < 0)
    return;
  else if (_lastError != length)
    _lastError = LIBUSB_ERROR_OTHER;
  else
    _lastError = 0;
}
