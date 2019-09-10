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
    Interface(dev, CONNECTION_USB)
{
    mediator = new PololuUsbConnectionMediator();

    const size_t MAX_SERIAL_NUMBER = 20; // serial number has 8 characters, 20 is super safe
    char serialNumber[MAX_SERIAL_NUMBER];    

    if (IUGetConfigText(dev->getDeviceName(), "TIC_SERIAL_TP", "TIC_SERIAL_NUMBER", serialNumber, MAX_SERIAL_NUMBER)) {
        serialNumber[0] = '\0';
    }
    else {
    	requiredSerialNumber = serialNumber;
    }

    IUFillText(TicSerialNumberT, "TIC_SERIAL_NUMBER", "Tic Serial Number", serialNumber);
    IUFillTextVector(&TicSerialNumberTP, TicSerialNumberT, 1, getDeviceName(), "TIC_SERIAL_TP", "Tic Serial Number", CONNECTION_TAB,
                     IP_RW, 60, IPS_IDLE);
};

PololuUsbConnection::~PololuUsbConnection() 
{
    delete mediator;
};

bool PololuUsbConnection::Connect() 
{ 
    if (!mediator->connect( requiredSerialNumber.c_str() )) 
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

    LOGF_INFO("Connected to Tic with serial: %s",mediator->getSerialNumber());

    TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
    IUSaveText(TicSerialNumberT, mediator->getSerialNumber());
    IDSetText(&TicSerialNumberTP, nullptr);

    return true;
};

bool PololuUsbConnection::Disconnect() 
{ 
    bool err = mediator->disconnect();
    if (err)
        LOGF_ERROR("Disconnecting error: %s.", mediator->getLastErrorMsg());

    IUSaveText(TicSerialNumberT, requiredSerialNumber.c_str());
    TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
    IDSetText(&TicSerialNumberTP, nullptr);

    return err;
};

void PololuUsbConnection::Activated() 
{
    m_Device->defineText(&TicSerialNumberTP);
};

void PololuUsbConnection::Deactivated() 
{
    m_Device->deleteProperty(TicSerialNumberTP.name);
};

bool PololuUsbConnection::saveConfigItems(FILE *fp) {

    if (!Connection::Interface::saveConfigItems(fp))
        return false;

    if (!requiredSerialNumber.empty()) {

        // make sure we are storing requiredSerialNumber as TicSerialNumberT may contain connected TIC serial
        char* tmpText = TicSerialNumberT[0].text;
        TicSerialNumberT[0].text = const_cast<char*>(requiredSerialNumber.c_str());

        IUSaveConfigText(fp,&TicSerialNumberTP);

        TicSerialNumberT[0].text = tmpText;
    }

    return true;
}

bool PololuUsbConnection::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (!strcmp(dev, m_Device->getDeviceName()))
    {
        if (!strcmp(name, TicSerialNumberTP.name)) {
            
            if (requiredSerialNumber == texts[0])
                return true;

            requiredSerialNumber = texts[0];

            if (m_Device->isConnected()) {

                if (requiredSerialNumber.empty()) {
                    TicSerialNumberTP.s = IPS_IDLE;
                }
                else {
                    LOG_WARN("Serial number selected. You must reconnect TicFocuser.");
                    TicSerialNumberTP.s = IPS_BUSY;                    
                }

            }
            else {
                IUUpdateText(&TicSerialNumberTP, texts, names, n);
                TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
            }
            
            IDSetText(&TicSerialNumberTP, nullptr);

            return true;
        }
    }

    return Connection::Interface::ISNewText(dev,name,texts,names,n);
}

TicMediator& PololuUsbConnection::getTicMediator() 
{ 
    return *mediator; 
}

