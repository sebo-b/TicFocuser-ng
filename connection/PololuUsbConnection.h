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

#ifndef TICCONNECTION_H
#define TICCONNECTION_H

#include <connectionplugins/connectioninterface.h>
#include "TicConnectionInterface.h"

struct tic_handle;
class PololuUsbConnectionMediator;

class PololuUsbConnection: 
    public Connection::Interface,
    public TicConnectionInterface
{
    public:

        PololuUsbConnection(INDI::DefaultDevice *dev);
        ~PololuUsbConnection();
        
        bool Connect();
        bool Disconnect();
        void Activated();
        void Deactivated();

        bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);

        std::string name() { return "Pololu USB Connection"; };
        std::string label() { return "Pololu USB"; };

        bool saveConfigItems(FILE *fp);

        TicMediator& getTicMediator();
        
    private:

        IText TicSerialNumberT[1] {};
        ITextVectorProperty TicSerialNumberTP;

        std::string requiredSerialNumber;

        PololuUsbConnectionMediator* mediator;
};

#endif // TICCONNECTION_H
