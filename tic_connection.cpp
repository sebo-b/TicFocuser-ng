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
#include "indilogger.h"

bool TicConnection::Connect() 
{ 

    //m_Device
    LOG_INFO("Connect");
    return true; 
};

/**
 * @brief Disconnect Disconnect from device.
 * @return True if successful, false otherwise.
 */
bool TicConnection::Disconnect() 
{ 
    LOG_INFO("Disconnect");
    return true;
};

/**
 * @brief Activated Function called by the framework when the plugin is activated (i.e. selected by the user). It is usually used to define properties
 * pertaining to the specific plugin functionalities.
 */
void TicConnection::Activated() {};

/**
 * @brief Deactivated Function called by the framework when the plugin is deactivated. It is usually used to delete properties by were defined
 * previously since the plugin is no longer active.
 */
void TicConnection::Deactivated() {};


TicConnection::TicConnection(INDI::DefaultDevice *dev):
    Interface(dev, CONNECTION_USB) 
    {

    };

TicConnection::~TicConnection() {};