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

#ifndef TICBASECONNECTIONMEDIATOR_H
#define TICBASECONNECTIONMEDIATOR_H

#include "TicMediatorInterface.h"

#error

class TicBase;

class TicBaseConnectionMediator: public TicMediatorInterface
{
protected:

    TicBase* ticBase;

public:

    bool energize();
    bool deenergize();
    bool exitSafeStart();
    bool haltAndHold();
    bool setTargetPosition(int position);
    bool haltAndSetPosition(int position);

    bool getVariables(TicMediatorInterface::TicVariables* vars);

};

#endif // TICBASECONNECTIONMEDIATOR_H