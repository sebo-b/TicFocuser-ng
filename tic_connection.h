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

#ifndef TICCONNECTION_H
#define TICCONNECTION_H

#include <connectionplugins/connectioninterface.h>

struct tic_handle;

class TicConnection: public Connection::Interface {
    
    public:

        TicConnection(INDI::DefaultDevice *dev);
        ~TicConnection();
        
        bool Connect();
        bool Disconnect();
        void Activated();
        void Deactivated();

        bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);

        tic_handle* getHandle() { return handle; }

        std::string name() { return "TicFocuser USB Connection"; };
        std::string label() { return "TicUSB"; };

    private:

        tic_handle* handle;

        IText TicSerialNumberT[1] {};
        ITextVectorProperty TicSerialNumberTP;

};

#endif // TICCONNECTION_H
