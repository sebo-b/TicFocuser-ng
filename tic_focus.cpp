
/*******************************************************************************
  Copyright(c) 2019 Helge Kutzop 

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
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include "tic_focus.h"
#include <tic.hpp>
#include <iostream>
//neu
#include <fstream>
#include <string>

using namespace tic;
using namespace std;
handle hnd;

// We declare an auto pointer to focusRpi.
std::unique_ptr<FocusTic> focusTic(new FocusTic());

#define MAX_STEPS 10000 // maximum steps focuser can travel from min=0 to max
#define STEP_DELAY 4 // miliseconds

//Identify TIC device

handle open_handle(string desired_serial_number)
{
  handle result = handle();
 
  // Get a list of Tic devices connected via USB.
  vector<device> list = list_connected_devices();
  cout << list.size() << " devices found" << endl;
  
  // Iterate through the list and select one device.
  device * dev = NULL;
  for (size_t i = 0; i < list.size(); i++)
  {
    device * candidate = &list[i];
    if (desired_serial_number.length() > 0)
    {
      string serial_number = candidate->get_serial_number();
      if (serial_number != desired_serial_number)
      {
        // Found a device with the wrong serial number, so continue on to
        // the next device in the list.
        continue;
      }
    }
 
    // Select this device as the one we want to connect to, and break
    // out of the loop.
    dev = candidate;
    break;
  }
 
  if (dev == NULL)
  {
    cerr << "Error: No device found." << endl;
  }
  else 
    result = handle(*dev);
 
  return result;
} 

void ISPoll(void *p);


void ISInit()
{
   static int isInit = 0;

   if (isInit == 1)
       return;
   if(focusTic.get() == 0)
   {
       isInit = 1;
       focusTic.reset(new FocusTic());
   }
}

void ISGetProperties(const char *dev)
{
        ISInit();
        focusTic->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
        ISInit();
        focusTic->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
        ISInit();
        focusTic->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
        ISInit();
        focusTic->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
  INDI_UNUSED(dev);
  INDI_UNUSED(name);
  INDI_UNUSED(sizes);
  INDI_UNUSED(blobsizes);
  INDI_UNUSED(blobs);
  INDI_UNUSED(formats);
  INDI_UNUSED(names);
  INDI_UNUSED(n);
}

void ISSnoopDevice (XMLEle *root)
{
    ISInit();
    focusTic->ISSnoopDevice(root);
}

FocusTic::FocusTic()
{
	setVersion(1,0);
        FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE); 
}

FocusTic::~FocusTic()
{

}

const char * FocusTic::getDefaultName()
{
        return (char *)"TIC Focuser";
}

bool FocusTic::Connect()
{
    // Start Work in Progress

    string test_desired_serial_ID;
    // the file "TIC_serial_ID" just contains a number, such as 1234
    // purpose is to allow the user to capture here the TIC serial ID
    // why specifying? in case the user runs more than one TIC, e.g. another TIC for the filter wheel
    ifstream link_serial_ID ("/home/helge/TIC_serial_ID");
    link_serial_ID.is_open();
    getline(link_serial_ID, test_desired_serial_ID);
    IDMessage(getDeviceName() , "Candidate is %s",test_desired_serial_ID.c_str());
    link_serial_ID.close();

    // End Work in Progress
  
    IDMessage(getDeviceName(), "TicFocuser connected successfully.");
    hnd = open_handle("");
    return true;
}

bool FocusTic::Disconnect()
{
	// park focuser
	if ( FocusParkingS[0].s == ISS_ON )
	{
		IDMessage(getDeviceName(), "TicFocuser is parking...");
		MoveAbsFocuser(FocusAbsPosN[0].min);
	}
    
	IDMessage(getDeviceName(), "TicFocuser disconnected successfully.");
        return true;
}

bool FocusTic::initProperties()
{
    INDI::Focuser::initProperties();
    
    IUFillNumber(&FocusAbsPosN[0],"FOCUS_ABSOLUTE_POSITION","Ticks","%0.0f",0,MAX_STEPS,(int)MAX_STEPS/100,0);
    IUFillNumberVector(&FocusAbsPosNP,FocusAbsPosN,1,getDeviceName(),"ABS_FOCUS_POSITION","Position",MAIN_CONTROL_TAB,IP_RW,0,IPS_OK);

        /* Step Mode */
        IUFillSwitch(&StepModeS[0], "Half Step", "", ISS_OFF);
        IUFillSwitch(&StepModeS[1], "Full Step", "", ISS_ON);
        IUFillSwitchVector(&StepModeSP, StepModeS, 2, getDeviceName(), "Step Mode", "", OPTIONS_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

	IUFillNumber(&PresetN[0], "Preset 1", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumber(&PresetN[1], "Preset 2", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumber(&PresetN[2], "Preset 3", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumberVector(&PresetNP, PresetN, 3, getDeviceName(), "Presets", "Presets", "Presets", IP_RW, 0, IPS_IDLE);

	IUFillSwitch(&PresetGotoS[0], "Preset 1", "Preset 1", ISS_OFF);
	IUFillSwitch(&PresetGotoS[1], "Preset 2", "Preset 2", ISS_OFF);
	IUFillSwitch(&PresetGotoS[2], "Preset 3", "Preset 3", ISS_OFF);
	IUFillSwitchVector(&PresetGotoSP, PresetGotoS, 3, getDeviceName(), "Presets Goto", "Goto", MAIN_CONTROL_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

	IUFillNumber(&FocusBacklashN[0], "FOCUS_BACKLASH_VALUE", "Steps", "%0.0f", 0, (int)(MAX_STEPS/100), (int)(MAX_STEPS/1000), 0);
	IUFillNumberVector(&FocusBacklashNP, FocusBacklashN, 1, getDeviceName(), "FOCUS_BACKLASH", "Backlash", OPTIONS_TAB, IP_RW, 0, IPS_IDLE);

	IUFillSwitch(&FocusResetS[0],"FOCUS_RESET","Reset",ISS_OFF);
	IUFillSwitchVector(&FocusResetSP,FocusResetS,1,getDeviceName(),"FOCUS_RESET","Position Reset",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

	IUFillSwitch(&FocusParkingS[0],"FOCUS_PARKON","Enable",ISS_OFF);
	IUFillSwitch(&FocusParkingS[1],"FOCUS_PARKOFF","Disable",ISS_OFF);
	IUFillSwitchVector(&FocusParkingSP,FocusParkingS,2,getDeviceName(),"FOCUS_PARK","Parking Mode",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

    return true;
}

void FocusTic::ISGetProperties (const char *dev)
{
    INDI::Focuser::ISGetProperties(dev);

    /* Add debug controls so we may debug driver if necessary */
    addDebugControl();

    return;
}

bool FocusTic::updateProperties()
{

    INDI::Focuser::updateProperties();

    if (isConnected())
    {
		deleteProperty(FocusSpeedNP.name);
        defineNumber(&FocusAbsPosNP);
        defineSwitch(&FocusMotionSP);
        defineSwitch(&StepModeSP);
		defineNumber(&FocusBacklashNP);
		defineSwitch(&FocusParkingSP);
		defineSwitch(&FocusResetSP);
    }
    else
    {
        deleteProperty(FocusAbsPosNP.name);
        deleteProperty(FocusMotionSP.name);
		deleteProperty(FocusBacklashNP.name);
		deleteProperty(FocusParkingSP.name);
		deleteProperty(FocusResetSP.name);
    }

    return true;
}

bool FocusTic::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{
	// first we check if it's for our device
	if(strcmp(dev,getDeviceName())==0)
	{

        // handle focus absolute position
        if (!strcmp(name, FocusAbsPosNP.name))
        {
			int newPos = (int) values[0];
            if ( MoveAbsFocuser(newPos) == IPS_OK )
            {
               IUUpdateNumber(&FocusAbsPosNP,values,names,n);
               FocusAbsPosNP.s=IPS_OK;
               IDSetNumber(&FocusAbsPosNP, NULL);
            }
            return true;
        }        

        // handle focus relative position
        if (!strcmp(name, FocusRelPosNP.name))
        {
			IUUpdateNumber(&FocusRelPosNP,values,names,n);
			
			//FOCUS_INWARD
            if ( FocusMotionS[0].s == ISS_ON )
				MoveRelFocuser(FOCUS_INWARD, FocusRelPosN[0].value);

			//FOCUS_OUTWARD
            if ( FocusMotionS[1].s == ISS_ON )
				MoveRelFocuser(FOCUS_OUTWARD, FocusRelPosN[0].value);

			FocusRelPosNP.s=IPS_OK;
			IDSetNumber(&FocusRelPosNP, NULL);
			return true;
        }

        // handle focus timer
        if (!strcmp(name, FocusTimerNP.name))
        {
			IUUpdateNumber(&FocusTimerNP,values,names,n);

			//FOCUS_INWARD
            if ( FocusMotionS[0].s == ISS_ON )
				MoveFocuser(FOCUS_INWARD, 0, FocusTimerN[0].value);

			//FOCUS_OUTWARD
            if ( FocusMotionS[1].s == ISS_ON )
				MoveFocuser(FOCUS_OUTWARD, 0, FocusTimerN[0].value);

			FocusTimerNP.s=IPS_OK;
			IDSetNumber(&FocusTimerNP, NULL);
			return true;
        }

        // handle focus backlash
        if (!strcmp(name, FocusBacklashNP.name))
        {
            IUUpdateNumber(&FocusBacklashNP,values,names,n);
            FocusBacklashNP.s=IPS_OK;
            IDSetNumber(&FocusBacklashNP, "TicFocuser backlash set to %d", (int) FocusBacklashN[0].value);
            return true;
        }
	}
    return INDI::Focuser::ISNewNumber(dev,name,values,names,n);
}

bool FocusTic::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
    // first we check if it's for our device
    if (!strcmp(dev, getDeviceName()))
    {
		
        // handle focus motion in and out
        if (!strcmp(name, FocusMotionSP.name))
        {
            IUUpdateSwitch(&FocusMotionSP, states, names, n);

			//FOCUS_INWARD
            if ( FocusMotionS[0].s == ISS_ON )
				MoveRelFocuser(FOCUS_INWARD, FocusRelPosN[0].value);

			//FOCUS_OUTWARD
            if ( FocusMotionS[1].s == ISS_ON )
				MoveRelFocuser(FOCUS_OUTWARD, FocusRelPosN[0].value);

            //FocusMotionS[0].s = ISS_OFF;
            //FocusMotionS[1].s = ISS_OFF;

			FocusMotionSP.s = IPS_OK;
            IDSetSwitch(&FocusMotionSP, NULL);
            return true;
        }

        // handle focus presets
        if (!strcmp(name, PresetGotoSP.name))
        {
            IUUpdateSwitch(&PresetGotoSP, states, names, n);

			//Preset 1
            if ( PresetGotoS[0].s == ISS_ON )
				MoveAbsFocuser(PresetN[0].value);

			//Preset 2
            if ( PresetGotoS[1].s == ISS_ON )
				MoveAbsFocuser(PresetN[1].value);

			//Preset 2
            if ( PresetGotoS[2].s == ISS_ON )
				MoveAbsFocuser(PresetN[2].value);

			PresetGotoS[0].s = ISS_OFF;
			PresetGotoS[1].s = ISS_OFF;
			PresetGotoS[2].s = ISS_OFF;
			PresetGotoSP.s = IPS_OK;
            IDSetSwitch(&PresetGotoSP, NULL);
            return true;
        }
                
        // handle focus reset
        if(!strcmp(name, FocusResetSP.name))
        {
			IUUpdateSwitch(&FocusResetSP, states, names, n);

            if ( FocusResetS[0].s == ISS_ON && FocusAbsPosN[0].value == FocusAbsPosN[0].min  )
            {
				FocusAbsPosN[0].value = (int)MAX_STEPS/100;
				IDSetNumber(&FocusAbsPosNP, NULL);
				MoveAbsFocuser(0);
			}
            FocusResetS[0].s = ISS_OFF;
            IDSetSwitch(&FocusResetSP, NULL);
			return true;
		}

        // handle parking mode
        if(!strcmp(name, FocusParkingSP.name))
        {
			IUUpdateSwitch(&FocusParkingSP, states, names, n);
			IDSetSwitch(&FocusParkingSP, NULL);
			return true;
		}

        // Focus Step Mode 
        if (strcmp(StepModeSP.name, name) == 0)
        {
            bool rc = false;
            int current_mode = IUFindOnSwitchIndex(&StepModeSP);
             
            IUUpdateSwitch(&StepModeSP, states, names, n);
 
            int target_mode = IUFindOnSwitchIndex(&StepModeSP);
            if (current_mode == target_mode)
            {
                  StepModeSP.s = IPS_OK;
                  IDSetSwitch(&StepModeSP, nullptr);
            }
 
            if (target_mode == 0)
                 rc = setStepMode(FOCUS_HALF_STEP);
            else
                 rc = setStepMode(FOCUS_FULL_STEP);
  
            if (!rc)
            {
                 IUResetSwitch(&StepModeSP);
                 StepModeS[current_mode].s = ISS_ON;
                 StepModeSP.s = IPS_ALERT;
                 IDSetSwitch(&StepModeSP, nullptr);
                 return false;
             }
  
             StepModeSP.s = IPS_OK;
             IDSetSwitch(&StepModeSP, nullptr);
             return true;
        }

    }
    return INDI::Focuser::ISNewSwitch(dev,name,states,names,n);
}

bool FocusTic::ISSnoopDevice (XMLEle *root)
{
    return INDI::Focuser::ISSnoopDevice(root);
}

bool FocusTic::saveConfigItems(FILE *fp)
{
    IUSaveConfigNumber(fp, &FocusRelPosNP);
    IUSaveConfigNumber(fp, &PresetNP);
    IUSaveConfigNumber(fp, &FocusBacklashNP);
	IUSaveConfigSwitch(fp, &FocusParkingSP);

    if ( FocusParkingS[0].s == ISS_ON )
		IUSaveConfigNumber(fp, &FocusAbsPosNP);

    return true;
}

bool FocusTic::setStepMode(FocusStepMode mode)
  {
     if (mode == FOCUS_HALF_STEP)
        hnd.set_step_mode(2);
     else
        hnd.set_step_mode(1);
     return true;
  }


IPState FocusTic::MoveFocuser(FocusDirection dir, int speed, int duration)
{
    int ticks = (int) ( duration / STEP_DELAY);
    return 	MoveRelFocuser( dir, ticks);
}


IPState FocusTic::MoveRelFocuser(FocusDirection dir, int ticks)
{
    int targetTicks = FocusAbsPosN[0].value + (ticks * (dir == FOCUS_INWARD ? -1 : 1));
    return MoveAbsFocuser(targetTicks);
}

IPState FocusTic::MoveAbsFocuser(int targetTicks)
{

    if (targetTicks < FocusAbsPosN[0].min || targetTicks > FocusAbsPosN[0].max)
    {
        IDMessage(getDeviceName(), "Requested position is out of range.");
        return IPS_ALERT;
    }
    	
    if (targetTicks == FocusAbsPosN[0].value)
    {
        // IDMessage(getDeviceName(), "TicFocuser already in the requested position.");
        return IPS_OK;
    }

	// set focuser busy
	FocusAbsPosNP.s = IPS_BUSY;
	IDSetNumber(&FocusAbsPosNP, NULL);

	SetSpeed(2);
        
    // set direction

    const char* direction;    

    if (targetTicks > FocusAbsPosN[0].value)
    {
		direction = " outward ";
	}
    else
	{
		direction = " inward ";
	}

    IDMessage(getDeviceName() , "TicFocuser is moving %s", direction);

    int ticks = abs(targetTicks - FocusAbsPosN[0].value);

    hnd.exit_safe_start();
    hnd.set_target_position(targetTicks);

    // update abspos value and status
    IDSetNumber(&FocusAbsPosNP, "TicFocuser moved to position %0.0f", FocusAbsPosN[0].value);
  
    FocusAbsPosNP.s = IPS_OK;
    IDSetNumber(&FocusAbsPosNP, NULL);
	
    return IPS_OK;
}

bool FocusTic::SetSpeed(int speed)
{}

