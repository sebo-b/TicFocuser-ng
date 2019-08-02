
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
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <iostream>

#include <fstream>
#include <string>

#include "tic.hpp"

#include "tic_focus.h"
#include "tic_connection.h"

// TODO: remove
using namespace tic;
using namespace std;

// TODO: move to object
handle hnd;

// We declare an auto pointer to focusTic.
std::unique_ptr<FocusTic> focusTic(new FocusTic());


// TODO move to properties
#define MAX_STEPS 10000 // maximum steps focuser can travel from min=0 to max
#define STEP_DELAY 4 // miliseconds







// TODO move to object
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

void ISGetProperties(const char *dev)
{
    focusTic->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
    focusTic->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
    focusTic->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
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
    focusTic->ISSnoopDevice(root);
}

FocusTic::FocusTic()
{
	setVersion(1,0);
    setSupportedConnections(CONNECTION_NONE);
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_CAN_ABORT ); // TODO: implement can abort
}

FocusTic::~FocusTic()
{
    // TODO: free handle
}

const char * FocusTic::getDefaultName()
{
    return (char *)"TIC Focuser";
}

bool FocusTic::Connect()
{
    //TODO
    //hnd = open_handle("");  
    IDMessage(getDeviceName(), "TicFocuser connected successfully.");
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
    IUFillSwitch(&StepModeS[0], "Full Step", "", ISS_ON);
    IUFillSwitch(&StepModeS[1], "Half Step", "", ISS_OFF);
    IUFillSwitch(&StepModeS[2], "1/4 Step", "", ISS_OFF);
    IUFillSwitch(&StepModeS[3], "1/8 Step", "", ISS_OFF);
    IUFillSwitchVector(&StepModeSP, StepModeS, 4, getDeviceName(), "Step Mode", "", OPTIONS_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

	IUFillNumber(&PresetN[0], "Preset 1", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumber(&PresetN[1], "Preset 2", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumber(&PresetN[2], "Preset 3", "", "%0.0f", 0, MAX_STEPS, (int)(MAX_STEPS/100), 0);
	IUFillNumberVector(&PresetNP, PresetN, 3, getDeviceName(), "Presets", "Presets", "Presets", IP_RW, 0, IPS_IDLE);

	IUFillSwitch(&PresetGotoS[0], "Preset 1", "Preset 1", ISS_OFF);
	IUFillSwitch(&PresetGotoS[1], "Preset 2", "Preset 2", ISS_OFF);
	IUFillSwitch(&PresetGotoS[2], "Preset 3", "Preset 3", ISS_OFF);
	IUFillSwitchVector(&PresetGotoSP, PresetGotoS, 3, getDeviceName(), "Presets Goto", "Goto", MAIN_CONTROL_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

    // TODO: add min/max position

    // TODO: add serial number
    IUFillText(TicSerialNumberT, "TIC_SERIAL", "Tic Serial Number", "");
    IUFillTextVector(&TicSerialNumberTP, TicSerialNumberT, 1, getDeviceName(), "TIC_SERIAL", "Tic Serial Number", CONNECTION_TAB,
                     IP_RW, 60, IPS_IDLE);
    defineText(&TicSerialNumberTP);

    IUFillNumber(&dbgN[0], "TIC_DBGN", "Tic debug", "%d",10,20,1,10);
    IUFillNumberVector(&dbgNP, dbgN, 1, getDeviceName(), "TIC_DBGN_PROP", "Tic debug number", CONNECTION_TAB,
                     IP_RW, 2, IPS_IDLE);
    defineNumber(&dbgNP);

	IUFillSwitch(&FocusResetS[0],"FOCUS_RESET","Reset",ISS_OFF);
	IUFillSwitchVector(&FocusResetSP,FocusResetS,1,getDeviceName(),"FOCUS_RESET","Position Reset",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

	IUFillSwitch(&FocusParkingS[0],"FOCUS_PARKON","Enable",ISS_OFF);
	IUFillSwitch(&FocusParkingS[1],"FOCUS_PARKOFF","Disable",ISS_OFF);
	IUFillSwitchVector(&FocusParkingSP,FocusParkingS,2,getDeviceName(),"FOCUS_PARK","Parking Mode",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_OK);

    TicConnection* ticC = new TicConnection(this);
    registerConnection(ticC);


    return true;
}

bool FocusTic::updateProperties()
{

    INDI::Focuser::updateProperties();

    // TODO: check
    if (isConnected())
    {
        defineNumber(&FocusAbsPosNP);
        defineSwitch(&FocusMotionSP);
        defineSwitch(&StepModeSP);
		defineSwitch(&FocusParkingSP);
		defineSwitch(&FocusResetSP);

//        TicSerialNumberTP.p = IP_RO;
//        deleteProperty(TicSerialNumberTP.name);
//        defineText(&TicSerialNumberTP);
    }
    else
    {
        deleteProperty(FocusAbsPosNP.name);
        deleteProperty(FocusMotionSP.name);
        deleteProperty(StepModeSP.name);
		deleteProperty(FocusParkingSP.name);
		deleteProperty(FocusResetSP.name);

//        TicSerialNumberTP.p = IP_RW;
//        deleteProperty(TicSerialNumberTP.name);
//        defineText(&TicSerialNumberTP);
    }

    return true;
}

bool FocusTic::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{
    // TODO: check and clean up
	// first we check if it's for our device
	if(strcmp(dev,getDeviceName())==0)
	{
        //TODO: remove
        if (!strcmp(name, dbgNP.name))
        {
            IDMessage(getDeviceName(), "DBG number.");
               IUUpdateNumber(&dbgNP,values,names,n);
               dbgNP.s=IPS_OK;
               IDSetNumber(&dbgNP, NULL);
               return true;
        }
        
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
	}

    return INDI::Focuser::ISNewNumber(dev,name,values,names,n);
}

bool FocusTic::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
    // TODO: check and clean up
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
 
            rc = setStepMode(static_cast<FocusStepMode>(target_mode));
  
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

bool FocusTic::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    IDMessage(getDeviceName(), "SEBO: %s %s",dev,name);
    if (!strcmp(dev, getDeviceName()))
    {
        if (!strcmp(name, TicSerialNumberTP.name)) {
            IUUpdateText(&TicSerialNumberTP, texts, names, n);
            TicSerialNumberTP.s = IPS_OK;
            IDSetText(&TicSerialNumberTP, nullptr);

            if (isConnected())
                IDMessage(getDeviceName(), "You must reconnect TicFocuser.");

            return true;
        }
    
    }

    return INDI::Focuser::ISNewText(dev,name,texts,names,n);
}

bool FocusTic::saveConfigItems(FILE *fp)
{
    Focuser::saveConfigItems(fp);

    // TODO: check if only these
    IUSaveConfigNumber(fp, &FocusRelPosNP);
    IUSaveConfigNumber(fp, &PresetNP);
	IUSaveConfigSwitch(fp, &FocusParkingSP);

    // TODO: possibly crappy thing
    if ( FocusParkingS[0].s == ISS_ON )
		IUSaveConfigNumber(fp, &FocusAbsPosNP);

    return true;
}

bool FocusTic::setStepMode(FocusStepMode mode)
{
    hnd.set_step_mode(mode);

    return true;
}


IPState FocusTic::MoveFocuser(FocusDirection dir, int speed, int duration)
{
    INDI_UNUSED(dir);
    INDI_UNUSED(speed);
    INDI_UNUSED(duration);
    DEBUGDEVICE(m_defaultDevice->getDeviceName(), INDI::Logger::DBG_ERROR, "Focuser does not support timer based motion.");

    return IPS_ALERT;
}


IPState FocusTic::MoveRelFocuser(FocusDirection dir, int ticks)
{
    int targetTicks = FocusAbsPosN[0].value + (ticks * (dir == FOCUS_INWARD ? -1 : 1));
    return MoveAbsFocuser(targetTicks);
}


// TODO: clean up
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

    // TODO: set speed
    hnd.exit_safe_start();
    hnd.set_target_position(targetTicks);

    // update abspos value and status
    IDSetNumber(&FocusAbsPosNP, "TicFocuser moved to position %0.0f", FocusAbsPosN[0].value);
  
    FocusAbsPosNP.s = IPS_OK;
    IDSetNumber(&FocusAbsPosNP, NULL);
	
    return IPS_OK;
}


