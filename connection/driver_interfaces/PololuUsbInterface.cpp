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

#include "PololuUsbInterface.h"

#include <tic.h>
#include <cstring>

/*
class TicErrorWrapper {
    tic_error* error;
public:
    TicErrorWrapper(): error(nullptr)   {}
    ~TicErrorWrapper() { this = (tic_error*)nullptr; }

    tic_error* operator=(tic_error*) { if (error) tic_error_free(error); return error = e; }
    operator bool()                     { return error; }
};*/

bool PololuUsbInterface::energize()
{
    tic_error* err = tic_energize(handle);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";

	return true;
}

bool PololuUsbInterface::deenergize()
{
    tic_error* err = tic_deenergize(handle);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";
    
    return true;

}

bool PololuUsbInterface::exitSafeStart()
{
    tic_error* err = tic_exit_safe_start(handle);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";
    
    return true;
}

bool PololuUsbInterface::haltAndHold()
{
    tic_error* err = tic_halt_and_hold(handle);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";
    
    return true;
}

bool PololuUsbInterface::setTargetPosition(int position)
{
    tic_error* err = tic_set_target_position(handle, position);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";
    
    return true;
}

bool PololuUsbInterface::haltAndSetPosition(int position)
{
    tic_error* err = tic_halt_and_set_position(handle, position);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";
    
    return true;
}

bool PololuUsbInterface::getVariables(TicVariables* vars)
{
    tic_variables* variables = NULL;

    tic_error* err  = tic_get_variables(handle,&variables,false);
    if (err) {
        lastErrorMsg = tic_error_get_message(err);
        tic_error_free(err);
        return false;
    }
    lastErrorMsg = "OK";

    vars->currentPosition = tic_variables_get_current_position(variables);
    vars->targetPosition = tic_variables_get_target_position(variables);

    tic_variables_free(variables);
    return true;
}

#if 0
PololuUsbConnectionMediator::TicErrorWrapper::~TicErrorWrapper() 
{
	if (error)
		tic_error_free(error);
}

tic_error* PololuUsbConnectionMediator::TicErrorWrapper::operator=(tic_error* e)
{
	if (error)
		tic_error_free(error);
	return error = e;		
}

PololuUsbConnectionMediator::PololuUsbConnectionMediator():
	handle(NULL)
{
}

PololuUsbConnectionMediator::~PololuUsbConnectionMediator() 
{
	if (handle) {
		tic_handle_close(handle);
		handle = NULL;
	}
}

bool PololuUsbConnectionMediator::connect(const char* requiredSerialNumber)
{
    disconnect();

    tic_device** deviceList;

    error = tic_list_connected_devices(&deviceList,NULL);
    if (error)
    	return !error;

    for (tic_device** d = deviceList; *d; ++d) {

        const char* devSerial = tic_device_get_serial_number(*d);

        if (requiredSerialNumber == NULL || !strcmp(requiredSerialNumber,"") || !strcmp(requiredSerialNumber,devSerial))
        {   
            error = tic_handle_open(*d,&handle);
            if (error)
                continue;

            serialNumber = devSerial;
            break;
        }
    }

    for (tic_device** d = deviceList; *d; ++d)
        tic_device_free(*d);
    tic_list_free(deviceList);

    return handle != NULL;
}

bool PololuUsbConnectionMediator::disconnect()
{
    if (handle)
    	tic_handle_close(handle);

    handle = NULL;
    serialNumber.clear();

    return true;
}

bool PololuUsbConnectionMediator::energize()
{
    return !(error = tic_energize(handle));
}

bool PololuUsbConnectionMediator::deenergize()
{
	return !(error = tic_energize(handle));
}

bool PololuUsbConnectionMediator::exitSafeStart()
{
	return !(error = tic_exit_safe_start(handle));
}

bool PololuUsbConnectionMediator::haltAndHold()
{
	return !(error = tic_halt_and_hold(handle));
}

bool PololuUsbConnectionMediator::setTargetPosition(int position)
{
	return !(error = tic_set_target_position(handle, position));
}

bool PololuUsbConnectionMediator::haltAndSetPosition(int position)
{
	return !(error = tic_halt_and_set_position(handle, position));
}

bool PololuUsbConnectionMediator::getVariables(TicMediatorInterface::TicVariables* vars)
{
    tic_variables* variables = NULL;

    error = tic_get_variables(handle,&variables,false);
    if (error)
    	return !error;

	vars->currentPosition = tic_variables_get_current_position(variables);
	vars->targetPosition = tic_variables_get_target_position(variables);

    tic_variables_free(variables);
	return true;
}

const char* PololuUsbConnectionMediator::getLastErrorMsg()
{
	if (!error)
		return NULL;

	return tic_error_get_message(error);
}

const char* PololuUsbConnectionMediator::getSerialNumber()
{
    return serialNumber.c_str();
}
#endif
