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

#include <stdio.h>
#include <unistd.h>
#include <memory>
#include <string.h>
#include <iostream>

#include <fstream>
#include <string>

#include "tic.h"

#include "tic_focuser.h"
#include "tic_connection.h"

std::unique_ptr<TicFocuser> ticFocuser(new TicFocuser());

void ISGetProperties(const char *dev)
{
    ticFocuser->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
    ticFocuser->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
    ticFocuser->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
    ticFocuser->ISNewNumber(dev, name, values, names, num);
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
    ticFocuser->ISSnoopDevice(root);
}

TicFocuser::TicFocuser():
    lastTimerHitError(false)
{
	setVersion(1,0);
    setSupportedConnections(CONNECTION_NONE);
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_CAN_SYNC | FOCUSER_CAN_ABORT );
}

TicFocuser::~TicFocuser()
{
}

bool TicFocuser::initProperties()
{
    INDI::Focuser::initProperties();

    /* Step Mode */
    IUFillSwitch(&StepModeS[0], "FULL_STEP", "Full Step", ISS_ON);
    IUFillSwitch(&StepModeS[1], "HALF STEP", "Half Step", ISS_OFF);
    IUFillSwitch(&StepModeS[2], "1_4_STEP", "1/4 Step", ISS_OFF);
    IUFillSwitch(&StepModeS[3], "1_8_STEP", "1/8 Step", ISS_OFF);
    IUFillSwitchVector(&StepModeSP, StepModeS, 4, getDeviceName(), "FOCUSER_STEP_MODE", "Step Mode", OPTIONS_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

	IUFillSwitch(&FocusParkingModeS[0],"FOCUS_PARKON","Enable",ISS_OFF);
	IUFillSwitch(&FocusParkingModeS[1],"FOCUS_PARKOFF","Disable",ISS_ON);
	IUFillSwitchVector(&FocusParkingModeSP,FocusParkingModeS,2,getDeviceName(),"FOCUS_PARK_MODE","Parking Mode",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_IDLE);

    TicConnection* ticC = new TicConnection(this);
    registerConnection(ticC);


    return true;
}

bool TicFocuser::updateProperties()
{
    INDI::Focuser::updateProperties();

    if (isConnected())
    {
        defineSwitch(&StepModeSP);
        defineSwitch(&FocusParkingModeSP);
    }
    else
    {
        deleteProperty(StepModeSP.name);
        deleteProperty(FocusParkingModeSP.name);
    }

    return true;
}

bool TicFocuser::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{
    return INDI::Focuser::ISNewNumber(dev,name,values,names,n);
}

bool TicFocuser::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    // first we check if it's for our device
    if (!strcmp(dev, getDeviceName()))
    {	
        // handle parking mode
        if(!strcmp(name, FocusParkingModeSP.name))
        {
			IUUpdateSwitch(&FocusParkingModeSP, states, names, n);
            StepModeSP.s = IPS_OK;
			IDSetSwitch(&FocusParkingModeSP, NULL);
			return true;
		}

        // Focus Step Mode 
        if (strcmp(StepModeSP.name, name) == 0)
        {             
            IUUpdateSwitch(&StepModeSP, states, names, n);
            StepModeSP.s = IPS_OK;
            IDSetSwitch(&StepModeSP, nullptr);

            return true;
        }

    }
    return INDI::Focuser::ISNewSwitch(dev,name,states,names,n);
}

bool TicFocuser::saveConfigItems(FILE *fp)
{
    Focuser::saveConfigItems(fp);

    IUSaveConfigSwitch(fp, &StepModeSP);
	IUSaveConfigSwitch(fp, &FocusParkingModeSP);

    return true;
}

bool TicFocuser::Disconnect() 
{
    // park focuser
    if (FocusParkingModeS[0].s != ISS_ON) {
        LOG_INFO("Parking mode disabled, parking not performed.");
    }
    else {
        MoveAbsFocuser(0);
    }

    return Focuser::Disconnect();
}

void TicFocuser::TimerHit() 
{
    if (!isConnected())
        return;

    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();

    tic_error* err = NULL;
    tic_variables* variables = NULL;

    do {

        err = tic_get_variables(handle,&variables,false);
        if (err) break;

        int32_t currentPos = tic_variables_get_current_position(variables);
        FocusAbsPosN[0].value = currentPos;

        uint8_t planningMode = tic_variables_get_planning_mode(variables);
        FocusAbsPosNP.s = planningMode == TIC_PLANNING_MODE_OFF? IPS_OK: IPS_BUSY;

        //int32_t targetPos = tic_variables_get_target_position(variables);

    } while (0);

    if (err) {

        if (!lastTimerHitError) {
            const char* errMsg = tic_error_get_message(err);
            LOGF_ERROR("TIC error: %s", errMsg);
        }

        tic_error_free(err);
        lastTimerHitError = true;
    }
    else {
        lastTimerHitError = false;
    }

    tic_variables_free(variables);

    IDSetNumber(&FocusAbsPosNP, nullptr);

    SetTimer(POLLMS);
}

bool TicFocuser::SyncFocuser(uint32_t ticks) 
{
    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();
    
    tic_error* err = tic_halt_and_set_position(handle, ticks);
    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot abort TIC. Error: %s", errMsg);
        tic_error_free(err);
        return false;
    }

    return true;
}


bool TicFocuser::AbortFocuser() {

    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();
    
    tic_error* err = tic_halt_and_hold(handle);
    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot abort TIC. Error: %s", errMsg);
        tic_error_free(err);
        return false;
    }

    return true;
}

IPState TicFocuser::MoveFocuser(FocusDirection dir, int speed, uint16_t duration)
{
    INDI_UNUSED(dir);
    INDI_UNUSED(speed);
    INDI_UNUSED(duration);
    LOG_ERROR("Focuser does not support timer based motion.");

    return IPS_ALERT;
}

IPState TicFocuser::MoveRelFocuser(FocusDirection dir, uint32_t ticks)
{
    int32_t absTicks = FocusAbsPosN[0].value;
    if (dir == FOCUS_OUTWARD)
        absTicks += ticks;
    else
        absTicks -= ticks;

    IPState ret =  MoveAbsFocuser(absTicks);

    if (ret == IPS_OK) {
        FocusAbsPosN[0].value = absTicks;
        FocusAbsPosNP.s = IPS_OK;
        IDSetNumber(&FocusAbsPosNP, nullptr);

        LOGF_INFO("TicFocuser moving to position %d", absTicks);
    }

    return ret;
}

uint8_t TicFocuser::ticStepModeConvert(int mode) 
{

  uint8_t modesMap[] = { TIC_STEP_MODE_MICROSTEP1, TIC_STEP_MODE_MICROSTEP2, TIC_STEP_MODE_MICROSTEP4, TIC_STEP_MODE_MICROSTEP8 };

  if (mode < 0 || mode >= sizeof(modesMap)/sizeof(modesMap[0]))
    return modesMap[0];

  return modesMap[mode];
}

IPState TicFocuser::MoveAbsFocuser(uint32_t ticks)
{
    if (ticks < FocusAbsPosN[0].min || ticks > FocusAbsPosN[0].max)
    {
        LOG_ERROR("Requested position is out of range.");
        return IPS_ALERT;
    }
    	
    if (ticks == FocusAbsPosN[0].value)
    {
        return IPS_OK;
    }

    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();
    tic_error* err = NULL;

    do {

        err = tic_exit_safe_start(handle);
        if (err) break;

        int step_mode = IUFindOnSwitchIndex(&StepModeSP);
        uint8_t tic_step_mode = ticStepModeConvert(step_mode);
        err = tic_set_step_mode(handle, tic_step_mode);
        if (err) break;

        err = tic_set_target_position(handle, ticks);
        if (err) break; 

    }
    while (0);

    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot abort TIC. Error: %s", errMsg);
        tic_error_free(err);
        return IPS_OK; //TODO debug
        return IPS_ALERT;
    }
  
    return IPS_OK;
}
