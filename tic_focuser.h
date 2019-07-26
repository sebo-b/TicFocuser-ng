/*******************************************************************************
  Copyright(c) 2019 Sebastian Baberowski
  Copyright(c) 2019 Helge Kutzop
  Copyright(c) 2014 Radek Kaczorek  <rkaczorek AT gmail DOT com>

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

#ifndef TICFOCUSER_H
#define TICFOCUSER_H

#include <indifocuser.h>

class TicFocuser : public INDI::Focuser
{ 
    public:
        
        TicFocuser();
        virtual ~TicFocuser();

        const char *getDefaultName() {  return "TIC Focuser"; }

/*	
        typedef enum { 
            FOCUS_FULL_STEP = TIC_STEP_MODE_MICROSTEP1,
            FOCUS_HALF_STEP = TIC_STEP_MODE_MICROSTEP2,
            FOCUS_1_4_STEP  = TIC_STEP_MODE_MICROSTEP4,
            FOCUS_1_8_STEP  = TIC_STEP_MODE_MICROSTEP8,
        } FocusStepMode;
*/
        virtual bool initProperties();
        virtual bool updateProperties();        

        bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
        bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);

        bool saveConfigItems(FILE *fp);

        IPState MoveFocuser(FocusDirection dir, int speed, uint16_t duration);
        IPState MoveAbsFocuser(uint32_t ticks);
        IPState MoveRelFocuser(FocusDirection dir, uint32_t ticks);
        bool AbortFocuser();

        // works only if Parking is enabled
        IPState ParkFocuser();

    private:

        ISwitch StepModeS[4];
        ISwitchVectorProperty StepModeSP;
        
        ISwitch FocusParkingModeS[2];
        ISwitchVectorProperty FocusParkingModeSP;

        ISwitch FocusParkS[1];
        ISwitchVectorProperty FocusParkSP;

};

#endif // TICFOCUSER_H
