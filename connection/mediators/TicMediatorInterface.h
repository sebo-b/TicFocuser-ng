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

#ifndef TICMEDIATORINTERFACE_H
#define TICMEDIATORINTERFACE_H

class TicMediatorInterface
{
public:

	virtual ~TicMediatorInterface() {}

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;


	virtual bool energize() = 0;
	virtual bool deenergize() = 0;

	virtual bool exitSafeStart() = 0;
	virtual bool haltAndHold() = 0;

	virtual bool setTargetPosition(int position) = 0;
	virtual bool haltAndSetPosition(int position) = 0;

	class TicVariables {
	public:
		int currentPosition;
		int targetPosition;
	};

	virtual bool getVariables(TicVariables*) = 0;

	virtual const char* getLastErrorMsg() = 0;
};

#endif // TICMEDIATORINTERFACE_H
