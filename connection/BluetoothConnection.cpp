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

#include "BluetoothConnection.h"
#include "mediators/BluetoothConnectionMediator.h"

#include <indilogger.h>
#include <string.h>

BluetoothConnection::BluetoothConnection(INDI::DefaultDevice *dev):
    Interface(dev, CONNECTION_CUSTOM)
{
    const size_t MAX_BT_MAC = 40; // bt mac has 17 characters, 40 is super safe
    char btMacAddress[MAX_BT_MAC];    

    if (IUGetConfigText(dev->getDeviceName(), "BT_MAC_TP", "BT_MAC_ADDRESS", btMacAddress, MAX_BT_MAC)) {
        btMacAddress[0] = '\0';
    }
    else {
    	requiredBtMacAddress = btMacAddress;
    }
//    requiredBtMacAddress = "00:15:A6:00:43:14";
//    IUFillText(BtMacAddressT, "BT_MAC_ADDRESS", "Bluetooth MAC address", requiredBtMacAddress.c_str());

    IUFillText(BtMacAddressT, "BT_MAC_ADDRESS", "Bluetooth MAC address", btMacAddress);
    IUFillTextVector(&BtMacAddressTP, BtMacAddressT, 1, getDeviceName(), "BT_MAC_TP", "Bluetooth MAC address", CONNECTION_TAB,
                     IP_RW, 60, IPS_IDLE);
    LOG_WARN("IUFillText in BluetoothConnection");



    mediator = new BluetoothConnectionMediator();
}

BluetoothConnection::~BluetoothConnection() 
{
    delete mediator;
}

bool BluetoothConnection::Connect()
{
    BluetoothConnectionMediator* btMediator = static_cast<BluetoothConnectionMediator*>(mediator);
    
    if (!btMediator->connect( requiredBtMacAddress.c_str() )) 
    {
        LOGF_ERROR("Bluetooth connection error: %s",mediator->getLastErrorMsg());
        return false;
    }

    LOGF_INFO("Connected to Bluetooth device with mac: %s",requiredBtMacAddress.c_str());

    BtMacAddressTP.s = IPS_OK;
    IUSaveText(BtMacAddressT, requiredBtMacAddress.c_str());
    IDSetText(&BtMacAddressTP, nullptr);

    return true;
}

bool BluetoothConnection::Disconnect() 
{ 
    bool res = mediator->disconnect();
    if (!res)
        LOGF_ERROR("Disconnecting error: %s.", mediator->getLastErrorMsg());

    BtMacAddressTP.s = IPS_IDLE;
    IUSaveText(BtMacAddressT, requiredBtMacAddress.c_str());
    IDSetText(&BtMacAddressTP, nullptr);

    return res;
}

void BluetoothConnection::Activated() 
{
    m_Device->defineText(&BtMacAddressTP);
}

void BluetoothConnection::Deactivated() 
{
    m_Device->deleteProperty(BtMacAddressTP.name);
}

bool BluetoothConnection::saveConfigItems(FILE *fp) {

    if (!Connection::Interface::saveConfigItems(fp))
        return false;

    if (!requiredBtMacAddress.empty()) {

        // make sure we are storing requiredBtMacAddress as BtMacAddressT may contain connected BT mac
        char* tmpText = BtMacAddressT[0].text;
        BtMacAddressT[0].text = const_cast<char*>(requiredBtMacAddress.c_str());

        IUSaveConfigText(fp,&BtMacAddressTP);

        BtMacAddressT[0].text = tmpText;
    }

    return true;
}

bool BluetoothConnection::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (!strcmp(dev, m_Device->getDeviceName()))
    {
        if (!strcmp(name, BtMacAddressTP.name)) {
            
            if (requiredBtMacAddress == texts[0])
                return true;

            requiredBtMacAddress = texts[0];

            if (m_Device->isConnected()) {

                if (requiredBtMacAddress.empty()) {
                    BtMacAddressTP.s = IPS_IDLE;
                }
                else {
                    LOG_WARN("Serial number selected. You must reconnect TicFocuser.");
                    BtMacAddressTP.s = IPS_BUSY;                    
                }

            }
            else {
                IUUpdateText(&BtMacAddressTP, texts, names, n);
                BtMacAddressTP.s = requiredBtMacAddress.empty()? IPS_IDLE: IPS_OK;
            }
            
            IDSetText(&BtMacAddressTP, nullptr);

            return true;
        }
    }

    return Connection::Interface::ISNewText(dev,name,texts,names,n);
}
