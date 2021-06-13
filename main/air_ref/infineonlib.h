#ifndef UARTLIB_H_
#define UARTLIB_H_

#include <stdint.h>
#include "error.h"

#define CONCAT(a,b) ( (a<<8) | (b & 0xffff) )


#define CHECKSUM(df_p) ( (0x0000 \
- CONCAT(df_p[1],df_p[0]) \
- CONCAT(df_p[3],df_p[2]) \
- CONCAT(df_p[5],df_p[4]) ) & 0xffff )


typedef enum {
	motor_stopped=0,
	motor_starting,
	motor_running,
} infineon_motor_state_t;


typedef enum {
	motor_error_none=0,
	motor_error_unable_to_start,
	motor_error_unable_to_maintain_speed,
	motor_error_fault
} infineon_motor_errors_t;

typedef enum {
	communication_error_none=0,
	communication_error_unreachable
} infineon_communication_errors_t;

typedef enum{
	INDEX_MOTOR_FAULT =0,
	INDEX_MOTOR_SPEED ,
	INDEX_MOTOR_MOTOR_CURRENT,
	INDEX_MOTOR_TARGET_SPEED,
	INDEX_MOTOR_SIZE
} index_status_motor_t;

typedef enum{
	INDEX_PFC_FAULT =0,
	INDEX_PFC_STICAZZI ,
	INDEX_PFC_SIZE
} index_status_pfc_t;



typedef enum{
	FAULT_GATEKILL                     =   0,
	FAULT_DC_OVER_VOLTAGE_CRITICAL     =   1,
	FAULT_DC_OVER_VOLTAGE              =   2,
	FAULT_DC_UNDER_VOLTAGE             =   3,
	FAULT_PLL                          =   4,
	FAULT_OVER_TEMPERATURE             =   6,
	FAULT_ROTOR_LOCK                   =   7,
	FAULT_PHASE_LOSS                   =   8,
	FAULT_OVER_CPU_LOAD                =   10,
	FAULT_PARAMETER_LOAD               =   12,
	FAULT_UART_TIMEOUT                 =   13,
	FAULT_HALL_TIMEOUT                 =   14,
	FAULT_FALL_NVALID                  =   15,
} infineon_fault_t;

#define INFINEON_ERROR_NONE 0
#define INFINEON_ERROR_TIMEOUT -1
#define INFINEON_ERROR_CHECKSUM -2


#endif