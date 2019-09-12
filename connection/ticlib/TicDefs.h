// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#ifndef TICDEFS_H
#define TICDEFS_H

#include <cstdint>
#include <unistd.h>

// copied from pololu-tic-software/include/tic_protocol.h
// licensed on MIT license
#define TIC_VENDOR_ID 0x1FFB
#define TIC_PRODUCT_ID_T825 0x00B3
#define TIC_PRODUCT_ID_T834 0x00B5
#define TIC_PRODUCT_ID_T500 0x00BD
#define TIC_PRODUCT_ID_N825 0x00C3
#define TIC_PRODUCT_ID_T249 0x00C9

// copied from pololu-tic-software/tic/tic_internal.h
typedef struct tic_name
{
  const char * name;
  uint32_t code;
} tic_name;

// copied from pololu-tic-software/lib/tic_names.c
extern const tic_name tic_error_names_ui[];
extern const size_t tic_error_names_ui_size;

extern const tic_name tic_step_mode_names_ui[];

extern const tic_name tic_operation_state_names_ui[];

const char* tic_look_up_name(const tic_name* map, int code);

#endif // TICDEFS_H