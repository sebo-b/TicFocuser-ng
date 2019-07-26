/*******************************************************************************
  Copyright(c) 2019 Sebastian Baberowski
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 .
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 .
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#include "tic_connection.h"

#include <string.h>

#include <indilogger.h>
#include <tic.h>


TicConnection::TicConnection(INDI::DefaultDevice *dev):
    Interface(dev, CONNECTION_USB), handle(NULL)
{	
    IUFillText(TicSerialNumberT, "Tic Serial Number", "Tic Serial Number", "");
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

		if (!TicSerialNumberT->text || !*TicSerialNumberT->text || !strcmp(TicSerialNumberT->text,devSerial)) {
			
			e = tic_handle_open(*d,&handle);
			if (e) {
				const char* errMsg = tic_error_get_message(e);
				LOGF_ERROR("Cannot open handle to TIC device with serial: %s. Error: %s", devSerial, errMsg);
				tic_error_free(e);
				handle = NULL;
			}
			break;
		}
	}

	for (tic_device** d = deviceList; *d; ++d)
		tic_device_free(*d);
	tic_list_free(deviceList);

	if (!handle) {
		LOG_ERROR("No TIC device found.");
		return true; // TODO: debug only
	}

    return true;
};

bool TicConnection::Disconnect() 
{ 
	// park focuser
	ISState st = ISS_ON;
	char name[] = "PARK";
	char* names[] = { name };

	m_Device->ISNewSwitch( m_Device->getDeviceName(), "FOCUS_PARK", &st, names, 1);

	tic_handle_close(handle);
	handle = NULL;

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

bool TicConnection::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (!strcmp(dev, m_Device->getDeviceName()))
    {
        if (!strcmp(name, TicSerialNumberTP.name)) {
            IUUpdateText(&TicSerialNumberTP, texts, names, n);
            
            if (n == 1 && !strlen(texts[0]))
				TicSerialNumberTP.s = IPS_IDLE;
			else
				TicSerialNumberTP.s = IPS_OK;

            IDSetText(&TicSerialNumberTP, nullptr);

            if (m_Device->isConnected())
            	LOG_INFO("Serial number selected. You must reconnect TicFocuser.");

            return true;
        }
    }

    return Connection::Interface::ISNewText(dev,name,texts,names,n);
}

