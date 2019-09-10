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

#include <string.h>

#include <indilogger.h>
#include <tic.h>


TicConnection::TicConnection(INDI::DefaultDevice *dev):
    Interface(dev, CONNECTION_USB), handle(NULL)
{
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

TicConnection::~TicConnection() 
{
    tic_handle_close(handle);
};

bool TicConnection::Connect() 
{ 
    tic_device** deviceList;
    tic_error *e = NULL;

    if (handle) {
        LOG_ERROR("Not NULL handle in TicConnection::Connect. Something is very wrong.");
        return false;    
    }

    e = tic_list_connected_devices(&deviceList,NULL);

    if (e) {
        const char* errMsg = tic_error_get_message(e);
        LOGF_ERROR("Cannot get list of connected TIC devices. Error: %s", errMsg);
        tic_error_free(e);
        return false;
    }

    for (tic_device** d = deviceList; *d; ++d) {

        const char* devSerial = tic_device_get_serial_number(*d);

        if (requiredSerialNumber.empty() || requiredSerialNumber == devSerial) {
            
            e = tic_handle_open(*d,&handle);
            if (e) {
                const char* errMsg = tic_error_get_message(e);
                LOGF_ERROR("Cannot open handle to TIC device with serial: %s. Error: %s", devSerial, errMsg);
                tic_error_free(e);
                handle = NULL;
            }

            TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
            IUSaveText(TicSerialNumberT, devSerial);
            IDSetText(&TicSerialNumberTP, nullptr);

            break;
        }
    }

    for (tic_device** d = deviceList; *d; ++d)
        tic_device_free(*d);
    tic_list_free(deviceList);

    if (!handle) {
        if (requiredSerialNumber.empty())
            LOG_ERROR("No TIC device found.");
        else 
            LOGF_ERROR("No TIC device found with serial: %s. You can set serial to empty to connect to the first found Tic device.", requiredSerialNumber.c_str());
        return false;
    }

    return true;
};

bool TicConnection::Disconnect() 
{ 
    tic_handle_close(handle);
    handle = NULL;

    IUSaveText(TicSerialNumberT, requiredSerialNumber.c_str());
    TicSerialNumberTP.s = requiredSerialNumber.empty()? IPS_IDLE: IPS_OK;
    IDSetText(&TicSerialNumberTP, nullptr);

    return true;
};

void TicConnection::Activated() 
{
    m_Device->defineText(&TicSerialNumberTP);
};

void TicConnection::Deactivated() 
{
    m_Device->deleteProperty(TicSerialNumberTP.name);
};

bool TicConnection::saveConfigItems(FILE *fp) {

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

bool TicConnection::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
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

