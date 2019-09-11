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

#include "LibUsbConnectionMediator.h"
#include <cstring>

#include "tic/TicUsb.h"


LibUsbConnectionMediator::LibUsbConnectionMediator()
{
    ticBase = new TicUsb();
}

LibUsbConnectionMediator::~LibUsbConnectionMediator() 
{
    delete ticBase;
}

bool LibUsbConnectionMediator::connect(const char* requiredSerialNumber)
{
    static_cast<TicUsb*>(ticBase)->connect(requiredSerialNumber);
    return !ticBase->getLastError();
}

bool LibUsbConnectionMediator::disconnect()
{
	static_cast<TicUsb*>(ticBase)->disconnect();
    return true;
}

const char* LibUsbConnectionMediator::getLastErrorMsg()
{
    return static_cast<TicUsb*>(ticBase)->getLastErrorMsg();
}

const char* LibUsbConnectionMediator::getSerialNumber()
{
	return static_cast<TicUsb*>(ticBase)->getSerial();
}