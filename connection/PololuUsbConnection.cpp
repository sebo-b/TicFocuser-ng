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

#include "PololuUsbConnection.h"

#include "mediators/PololuUsbConnectionMediator.h"
#include <string.h>

#include <indilogger.h>
#include <tic.h>

PololuUsbConnection::PololuUsbConnection(INDI::DefaultDevice *dev):
    UsbConnectionBase("PUSB_SERIAL_NUMBER",dev)
{
    mediator = new PololuUsbConnectionMediator();
};

PololuUsbConnection::~PololuUsbConnection() 
{
    delete mediator;
};

bool PololuUsbConnection::Connect() 
{ 
    PololuUsbConnectionMediator* usbMediator = static_cast<PololuUsbConnectionMediator*>(mediator);
    
    if (!usbMediator->connect( requiredSerialNumber.c_str() )) 
    {
        const char* errorMsg = mediator->getLastErrorMsg();

        if (errorMsg) 
        {
            LOGF_ERROR("Tic error: %s",errorMsg);
        }
        else
        {
            if (requiredSerialNumber.empty())
                LOG_ERROR("No TIC device found.");
            else
                LOGF_ERROR("No TIC device found with serial: %s. You can set serial to empty to connect to the first found Tic device.", requiredSerialNumber.c_str());
        }

        return false;
    }

    LOGF_INFO("Connected to Tic with serial: %s",usbMediator->getSerialNumber());

    TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
    IUSaveText(TicSerialNumberT, usbMediator->getSerialNumber());
    IDSetText(&TicSerialNumberTP, nullptr);

    return true;
};
