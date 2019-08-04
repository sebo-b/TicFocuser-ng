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

void ISNewText(    const char *dev, const char *name, char *texts[], char *names[], int num)
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
    lastTimerHitError(false), moveRelInitialValue(-1)
{
    setVersion(0,9);
    setSupportedConnections(CONNECTION_NONE);
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_CAN_SYNC | FOCUSER_CAN_ABORT );
}

TicFocuser::~TicFocuser()
{
}

bool TicFocuser::initProperties()
{
    INDI::Focuser::initProperties();

    IUFillSwitch(&FocusParkingModeS[0],"FOCUS_PARKON","Enable",ISS_OFF);
    IUFillSwitch(&FocusParkingModeS[1],"FOCUS_PARKOFF","Disable",ISS_ON);
    IUFillSwitchVector(&FocusParkingModeSP,FocusParkingModeS,2,getDeviceName(),"FOCUS_PARK_MODE","Parking Mode",OPTIONS_TAB,IP_RW,ISR_1OFMANY,60,IPS_IDLE);

    IUFillSwitch(&EnergizeFocuserS[0],"ENERGIZE_FOCUSER","Energize focuser",ISS_OFF);
    IUFillSwitch(&EnergizeFocuserS[1],"DEENERGIZE_FOCUSER","De-energize focuser",ISS_OFF);
    IUFillSwitchVector(&EnergizeFocuserSP,EnergizeFocuserS,2,getDeviceName(),"ENERGIZE_FOCUSER","Energize",MAIN_CONTROL_TAB,IP_RW,ISR_1OFMANY,60,IPS_IDLE);

    TicConnection* ticC = new TicConnection(this);
    registerConnection(ticC);

    return true;
}

bool TicFocuser::updateProperties()
{
    INDI::Focuser::updateProperties();

    if (isConnected())
    {
        defineSwitch(&EnergizeFocuserSP);
        defineSwitch(&FocusParkingModeSP);
    }
    else
    {
        deleteProperty(FocusParkingModeSP.name);
        deleteProperty(EnergizeFocuserSP.name);
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
            FocusParkingModeSP.s = IPS_OK;
            IDSetSwitch(&FocusParkingModeSP, NULL);
            return true;
        }

        if(!strcmp(name, EnergizeFocuserSP.name))
        {
            bool res;

            if (!strcmp(names[0],EnergizeFocuserS[0].name))
                res = energizeFocuser();
            else
                res = deenergizeFocuser();

            EnergizeFocuserSP.s = res? IPS_OK: IPS_ALERT;
            IDSetSwitch(&EnergizeFocuserSP, NULL);

            return true;
        }


    }
    return INDI::Focuser::ISNewSwitch(dev,name,states,names,n);
}

bool TicFocuser::saveConfigItems(FILE *fp)
{
    if (!Focuser::saveConfigItems(fp))
        return false;

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

bool TicFocuser::Connect() 
{
    bool res = Focuser::Connect();

    if (res) {
        energizeFocuser();  // Error will be shown by energizeFocuser() function, no need to show it here
    }

    return res;
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
        FocusSyncN[0].value = currentPos;

//        uint8_t planningMode = tic_variables_get_planning_mode(variables);
//        FocusAbsPosNP.s = planningMode == TIC_PLANNING_MODE_OFF? IPS_OK: IPS_BUSY;

        if (FocusAbsPosNP.s == IPS_BUSY) {

            if (moveRelInitialValue >= 0) {
                FocusRelPosN[0].value = abs( moveRelInitialValue - currentPos);
            }

            int32_t targetPos = tic_variables_get_target_position(variables);

            if (currentPos == targetPos) {
                FocusAbsPosNP.s = IPS_OK;
                FocusRelPosNP.s = IPS_OK;
                moveRelInitialValue = -1;
            }
        }

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
    IDSetNumber(&FocusRelPosNP, nullptr);
    IDSetNumber(&FocusSyncNP, nullptr);

    SetTimer(POLLMS);
}

bool TicFocuser::energizeFocuser()
{
    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();
    
    tic_error* err = NULL;

    do {

        err = tic_energize(handle);
        if (err) break;

        err = tic_exit_safe_start(handle);
        if (err) break;        

    } while (0);

    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot energize motor. Error: %s", errMsg);
        tic_error_free(err);
        return false;
    }

    return true;
}

bool TicFocuser::deenergizeFocuser()
{
    TicConnection* conn = static_cast<TicConnection*>(getActiveConnection());    
    tic_handle* handle = conn->getHandle();
    
    tic_error* err = tic_deenergize(handle);

    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot de-energize motor. Error: %s", errMsg);
        tic_error_free(err);
        return false;
    }

    LOG_INFO("Focuser de-energized. You must energize it to resume normal operation.");

    return true;
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
    int32_t targetTicks;
    if (dir == FOCUS_OUTWARD)
        targetTicks = absTicks + ticks;
    else
        targetTicks = absTicks - ticks;

    IPState ret =  MoveAbsFocuser(targetTicks);

    moveRelInitialValue = ret == IPS_BUSY? absTicks: -1;

    FocusAbsPosNP.s = ret;
    IDSetNumber(&FocusAbsPosNP, nullptr);

    return ret;
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

    tic_error* err = tic_set_target_position(handle, ticks);

    if (err) {
        const char* errMsg = tic_error_get_message(err);
        LOGF_ERROR("Cannot abort TIC. Error: %s", errMsg);
        tic_error_free(err);
        return IPS_ALERT;
    }
  
    return IPS_BUSY;
}
