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

#ifndef TICUSB_H
#define TICUSB_H

#include "TicBase.h"
#include <string>

struct libusb_device_handle;
struct libusb_context;

class TicUsb: public TicBase {

	libusb_device_handle *handle;
	libusb_context *context;
	std::string serialNumber;

public:

	TicUsb();
	~TicUsb();

	void connect(const char* serialNo);
	void disconnect();
	const char* getSerial()	{ return serialNumber.c_str(); }

	void commandQuick(TicCommand cmd);
	void commandW32(TicCommand cmd, uint32_t val);
	void commandW7(TicCommand cmd, uint8_t val);
	void getSegment(TicCommand cmd, uint8_t offset, uint8_t length, void * buffer);

	const char* getLastErrorMsg();
};

#endif // TICUSB_H