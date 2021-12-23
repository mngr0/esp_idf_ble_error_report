#ifndef AIRREF_READER_H
#define AIRREF_READER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef FLATBUFFERS_COMMON_READER_H
#include "flatbuffers_common_reader.h"
#endif
#ifndef BLUEFI_READER_H
#include "bluefi_reader.h"
#endif
#ifndef LOGGER_READER_H
#include "logger_reader.h"
#endif
#include "flatcc/flatcc_flatbuffers.h"
#ifndef __alignas_is_defined
#include <stdalign.h>
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif

typedef struct AirRef_ErrorReport AirRef_ErrorReport_t;
typedef const AirRef_ErrorReport_t *AirRef_ErrorReport_struct_t;
typedef AirRef_ErrorReport_t *AirRef_ErrorReport_mutable_struct_t;
typedef const AirRef_ErrorReport_t *AirRef_ErrorReport_vec_t;
typedef AirRef_ErrorReport_t *AirRef_ErrorReport_mutable_vec_t;
typedef struct AirRef_ErrorList AirRef_ErrorList_t;
typedef const AirRef_ErrorList_t *AirRef_ErrorList_struct_t;
typedef AirRef_ErrorList_t *AirRef_ErrorList_mutable_struct_t;
typedef const AirRef_ErrorList_t *AirRef_ErrorList_vec_t;
typedef AirRef_ErrorList_t *AirRef_ErrorList_mutable_vec_t;
typedef struct AirRef_MotorStatus AirRef_MotorStatus_t;
typedef const AirRef_MotorStatus_t *AirRef_MotorStatus_struct_t;
typedef AirRef_MotorStatus_t *AirRef_MotorStatus_mutable_struct_t;
typedef const AirRef_MotorStatus_t *AirRef_MotorStatus_vec_t;
typedef AirRef_MotorStatus_t *AirRef_MotorStatus_mutable_vec_t;

typedef const struct AirRef_AirRefConf_table *AirRef_AirRefConf_table_t;
typedef struct AirRef_AirRefConf_table *AirRef_AirRefConf_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_AirRefConf_vec_t;
typedef flatbuffers_uoffset_t *AirRef_AirRefConf_mutable_vec_t;
typedef const struct AirRef_AirRefState_table *AirRef_AirRefState_table_t;
typedef struct AirRef_AirRefState_table *AirRef_AirRefState_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_AirRefState_vec_t;
typedef flatbuffers_uoffset_t *AirRef_AirRefState_mutable_vec_t;
typedef const struct AirRef_MachineState_table *AirRef_MachineState_table_t;
typedef struct AirRef_MachineState_table *AirRef_MachineState_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_MachineState_vec_t;
typedef flatbuffers_uoffset_t *AirRef_MachineState_mutable_vec_t;
typedef const struct AirRef_Request_table *AirRef_Request_table_t;
typedef struct AirRef_Request_table *AirRef_Request_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_Request_vec_t;
typedef flatbuffers_uoffset_t *AirRef_Request_mutable_vec_t;
typedef const struct AirRef_LoggerState_table *AirRef_LoggerState_table_t;
typedef struct AirRef_LoggerState_table *AirRef_LoggerState_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_LoggerState_vec_t;
typedef flatbuffers_uoffset_t *AirRef_LoggerState_mutable_vec_t;
typedef const struct AirRef_Message_table *AirRef_Message_table_t;
typedef struct AirRef_Message_table *AirRef_Message_mutable_table_t;
typedef const flatbuffers_uoffset_t *AirRef_Message_vec_t;
typedef flatbuffers_uoffset_t *AirRef_Message_mutable_vec_t;
#ifndef AirRef_ErrorReport_file_identifier
#define AirRef_ErrorReport_file_identifier 0
#endif
/* deprecated, use AirRef_ErrorReport_file_identifier */
#ifndef AirRef_ErrorReport_identifier
#define AirRef_ErrorReport_identifier 0
#endif
#define AirRef_ErrorReport_type_hash ((flatbuffers_thash_t)0x69cc2486)
#define AirRef_ErrorReport_type_identifier "\x86\x24\xcc\x69"
#ifndef AirRef_ErrorReport_file_extension
#define AirRef_ErrorReport_file_extension "bin"
#endif
#ifndef AirRef_ErrorList_file_identifier
#define AirRef_ErrorList_file_identifier 0
#endif
/* deprecated, use AirRef_ErrorList_file_identifier */
#ifndef AirRef_ErrorList_identifier
#define AirRef_ErrorList_identifier 0
#endif
#define AirRef_ErrorList_type_hash ((flatbuffers_thash_t)0x3270156c)
#define AirRef_ErrorList_type_identifier "\x6c\x15\x70\x32"
#ifndef AirRef_ErrorList_file_extension
#define AirRef_ErrorList_file_extension "bin"
#endif
#ifndef AirRef_MotorStatus_file_identifier
#define AirRef_MotorStatus_file_identifier 0
#endif
/* deprecated, use AirRef_MotorStatus_file_identifier */
#ifndef AirRef_MotorStatus_identifier
#define AirRef_MotorStatus_identifier 0
#endif
#define AirRef_MotorStatus_type_hash ((flatbuffers_thash_t)0x5d858cd)
#define AirRef_MotorStatus_type_identifier "\xcd\x58\xd8\x05"
#ifndef AirRef_MotorStatus_file_extension
#define AirRef_MotorStatus_file_extension "bin"
#endif
#ifndef AirRef_AirRefConf_file_identifier
#define AirRef_AirRefConf_file_identifier 0
#endif
/* deprecated, use AirRef_AirRefConf_file_identifier */
#ifndef AirRef_AirRefConf_identifier
#define AirRef_AirRefConf_identifier 0
#endif
#define AirRef_AirRefConf_type_hash ((flatbuffers_thash_t)0xfa5f17a9)
#define AirRef_AirRefConf_type_identifier "\xa9\x17\x5f\xfa"
#ifndef AirRef_AirRefConf_file_extension
#define AirRef_AirRefConf_file_extension "bin"
#endif
#ifndef AirRef_AirRefState_file_identifier
#define AirRef_AirRefState_file_identifier 0
#endif
/* deprecated, use AirRef_AirRefState_file_identifier */
#ifndef AirRef_AirRefState_identifier
#define AirRef_AirRefState_identifier 0
#endif
#define AirRef_AirRefState_type_hash ((flatbuffers_thash_t)0x152c59c8)
#define AirRef_AirRefState_type_identifier "\xc8\x59\x2c\x15"
#ifndef AirRef_AirRefState_file_extension
#define AirRef_AirRefState_file_extension "bin"
#endif
#ifndef AirRef_MachineState_file_identifier
#define AirRef_MachineState_file_identifier 0
#endif
/* deprecated, use AirRef_MachineState_file_identifier */
#ifndef AirRef_MachineState_identifier
#define AirRef_MachineState_identifier 0
#endif
#define AirRef_MachineState_type_hash ((flatbuffers_thash_t)0x46f24f2c)
#define AirRef_MachineState_type_identifier "\x2c\x4f\xf2\x46"
#ifndef AirRef_MachineState_file_extension
#define AirRef_MachineState_file_extension "bin"
#endif
#ifndef AirRef_Request_file_identifier
#define AirRef_Request_file_identifier 0
#endif
/* deprecated, use AirRef_Request_file_identifier */
#ifndef AirRef_Request_identifier
#define AirRef_Request_identifier 0
#endif
#define AirRef_Request_type_hash ((flatbuffers_thash_t)0x188db0f)
#define AirRef_Request_type_identifier "\x0f\xdb\x88\x01"
#ifndef AirRef_Request_file_extension
#define AirRef_Request_file_extension "bin"
#endif
#ifndef AirRef_LoggerState_file_identifier
#define AirRef_LoggerState_file_identifier 0
#endif
/* deprecated, use AirRef_LoggerState_file_identifier */
#ifndef AirRef_LoggerState_identifier
#define AirRef_LoggerState_identifier 0
#endif
#define AirRef_LoggerState_type_hash ((flatbuffers_thash_t)0xe36a0ab3)
#define AirRef_LoggerState_type_identifier "\xb3\x0a\x6a\xe3"
#ifndef AirRef_LoggerState_file_extension
#define AirRef_LoggerState_file_extension "bin"
#endif
#ifndef AirRef_Message_file_identifier
#define AirRef_Message_file_identifier 0
#endif
/* deprecated, use AirRef_Message_file_identifier */
#ifndef AirRef_Message_identifier
#define AirRef_Message_identifier 0
#endif
#define AirRef_Message_type_hash ((flatbuffers_thash_t)0x8de4db49)
#define AirRef_Message_type_identifier "\x49\xdb\xe4\x8d"
#ifndef AirRef_Message_file_extension
#define AirRef_Message_file_extension "bin"
#endif

typedef int32_t AirRef_ErrorState_enum_t;
__flatbuffers_define_integer_type(AirRef_ErrorState, AirRef_ErrorState_enum_t, 32)
#define AirRef_ErrorState_error_state_none ((AirRef_ErrorState_enum_t)INT32_C(0))
#define AirRef_ErrorState_error_state_warning ((AirRef_ErrorState_enum_t)INT32_C(1))
#define AirRef_ErrorState_error_state_critical ((AirRef_ErrorState_enum_t)INT32_C(2))

static inline const char *AirRef_ErrorState_name(AirRef_ErrorState_enum_t value)
{
    switch (value) {
    case AirRef_ErrorState_error_state_none: return "error_state_none";
    case AirRef_ErrorState_error_state_warning: return "error_state_warning";
    case AirRef_ErrorState_error_state_critical: return "error_state_critical";
    default: return "";
    }
}

static inline int AirRef_ErrorState_is_known_value(AirRef_ErrorState_enum_t value)
{
    switch (value) {
    case AirRef_ErrorState_error_state_none: return 1;
    case AirRef_ErrorState_error_state_warning: return 1;
    case AirRef_ErrorState_error_state_critical: return 1;
    default: return 0;
    }
}

typedef int32_t AirRef_ControlType_enum_t;
__flatbuffers_define_integer_type(AirRef_ControlType, AirRef_ControlType_enum_t, 32)
#define AirRef_ControlType_control_type_manual_off ((AirRef_ControlType_enum_t)INT32_C(0))
#define AirRef_ControlType_control_type_manual_on ((AirRef_ControlType_enum_t)INT32_C(1))
#define AirRef_ControlType_control_type_auto ((AirRef_ControlType_enum_t)INT32_C(2))

static inline const char *AirRef_ControlType_name(AirRef_ControlType_enum_t value)
{
    switch (value) {
    case AirRef_ControlType_control_type_manual_off: return "control_type_manual_off";
    case AirRef_ControlType_control_type_manual_on: return "control_type_manual_on";
    case AirRef_ControlType_control_type_auto: return "control_type_auto";
    default: return "";
    }
}

static inline int AirRef_ControlType_is_known_value(AirRef_ControlType_enum_t value)
{
    switch (value) {
    case AirRef_ControlType_control_type_manual_off: return 1;
    case AirRef_ControlType_control_type_manual_on: return 1;
    case AirRef_ControlType_control_type_auto: return 1;
    default: return 0;
    }
}

typedef int32_t AirRef_AirRefIndexError_enum_t;
__flatbuffers_define_integer_type(AirRef_AirRefIndexError, AirRef_AirRefIndexError_enum_t, 32)
#define AirRef_AirRefIndexError_index_error_pressure_evap ((AirRef_AirRefIndexError_enum_t)INT32_C(0))
#define AirRef_AirRefIndexError_index_error_pressure_cond ((AirRef_AirRefIndexError_enum_t)INT32_C(1))
#define AirRef_AirRefIndexError_index_error_temperature_gas_scarico ((AirRef_AirRefIndexError_enum_t)INT32_C(2))
#define AirRef_AirRefIndexError_index_error_temperature_environment ((AirRef_AirRefIndexError_enum_t)INT32_C(3))
#define AirRef_AirRefIndexError_index_error_temperature_gas_ritorno ((AirRef_AirRefIndexError_enum_t)INT32_C(4))
#define AirRef_AirRefIndexError_index_error_temperature_extra ((AirRef_AirRefIndexError_enum_t)INT32_C(5))
#define AirRef_AirRefIndexError_index_error_imm101_motor ((AirRef_AirRefIndexError_enum_t)INT32_C(6))
#define AirRef_AirRefIndexError_index_error_imm101_communication ((AirRef_AirRefIndexError_enum_t)INT32_C(7))
#define AirRef_AirRefIndexError_index_error_imc102_motor ((AirRef_AirRefIndexError_enum_t)INT32_C(8))
#define AirRef_AirRefIndexError_index_error_imc102_communication ((AirRef_AirRefIndexError_enum_t)INT32_C(9))
#define AirRef_AirRefIndexError_index_error_overtemperature_gas_scarico ((AirRef_AirRefIndexError_enum_t)INT32_C(10))
#define AirRef_AirRefIndexError_index_low_pressure_protection ((AirRef_AirRefIndexError_enum_t)INT32_C(11))
#define AirRef_AirRefIndexError_index_error_size ((AirRef_AirRefIndexError_enum_t)INT32_C(12))

static inline const char *AirRef_AirRefIndexError_name(AirRef_AirRefIndexError_enum_t value)
{
    switch (value) {
    case AirRef_AirRefIndexError_index_error_pressure_evap: return "index_error_pressure_evap";
    case AirRef_AirRefIndexError_index_error_pressure_cond: return "index_error_pressure_cond";
    case AirRef_AirRefIndexError_index_error_temperature_gas_scarico: return "index_error_temperature_gas_scarico";
    case AirRef_AirRefIndexError_index_error_temperature_environment: return "index_error_temperature_environment";
    case AirRef_AirRefIndexError_index_error_temperature_gas_ritorno: return "index_error_temperature_gas_ritorno";
    case AirRef_AirRefIndexError_index_error_temperature_extra: return "index_error_temperature_extra";
    case AirRef_AirRefIndexError_index_error_imm101_motor: return "index_error_imm101_motor";
    case AirRef_AirRefIndexError_index_error_imm101_communication: return "index_error_imm101_communication";
    case AirRef_AirRefIndexError_index_error_imc102_motor: return "index_error_imc102_motor";
    case AirRef_AirRefIndexError_index_error_imc102_communication: return "index_error_imc102_communication";
    case AirRef_AirRefIndexError_index_error_overtemperature_gas_scarico: return "index_error_overtemperature_gas_scarico";
    case AirRef_AirRefIndexError_index_low_pressure_protection: return "index_low_pressure_protection";
    case AirRef_AirRefIndexError_index_error_size: return "index_error_size";
    default: return "";
    }
}

static inline int AirRef_AirRefIndexError_is_known_value(AirRef_AirRefIndexError_enum_t value)
{
    switch (value) {
    case AirRef_AirRefIndexError_index_error_pressure_evap: return 1;
    case AirRef_AirRefIndexError_index_error_pressure_cond: return 1;
    case AirRef_AirRefIndexError_index_error_temperature_gas_scarico: return 1;
    case AirRef_AirRefIndexError_index_error_temperature_environment: return 1;
    case AirRef_AirRefIndexError_index_error_temperature_gas_ritorno: return 1;
    case AirRef_AirRefIndexError_index_error_temperature_extra: return 1;
    case AirRef_AirRefIndexError_index_error_imm101_motor: return 1;
    case AirRef_AirRefIndexError_index_error_imm101_communication: return 1;
    case AirRef_AirRefIndexError_index_error_imc102_motor: return 1;
    case AirRef_AirRefIndexError_index_error_imc102_communication: return 1;
    case AirRef_AirRefIndexError_index_error_overtemperature_gas_scarico: return 1;
    case AirRef_AirRefIndexError_index_low_pressure_protection: return 1;
    case AirRef_AirRefIndexError_index_error_size: return 1;
    default: return 0;
    }
}

typedef int32_t AirRef_AirRefStatus_enum_t;
__flatbuffers_define_integer_type(AirRef_AirRefStatus, AirRef_AirRefStatus_enum_t, 32)
#define AirRef_AirRefStatus_status_idle ((AirRef_AirRefStatus_enum_t)INT32_C(0))
#define AirRef_AirRefStatus_status_running ((AirRef_AirRefStatus_enum_t)INT32_C(1))
#define AirRef_AirRefStatus_status_critical_error ((AirRef_AirRefStatus_enum_t)INT32_C(2))

static inline const char *AirRef_AirRefStatus_name(AirRef_AirRefStatus_enum_t value)
{
    switch (value) {
    case AirRef_AirRefStatus_status_idle: return "status_idle";
    case AirRef_AirRefStatus_status_running: return "status_running";
    case AirRef_AirRefStatus_status_critical_error: return "status_critical_error";
    default: return "";
    }
}

static inline int AirRef_AirRefStatus_is_known_value(AirRef_AirRefStatus_enum_t value)
{
    switch (value) {
    case AirRef_AirRefStatus_status_idle: return 1;
    case AirRef_AirRefStatus_status_running: return 1;
    case AirRef_AirRefStatus_status_critical_error: return 1;
    default: return 0;
    }
}

typedef int32_t AirRef_RequestType_enum_t;
__flatbuffers_define_integer_type(AirRef_RequestType, AirRef_RequestType_enum_t, 32)
#define AirRef_RequestType_RequestAirRefConf ((AirRef_RequestType_enum_t)INT32_C(0))
#define AirRef_RequestType_RequestAirRefState ((AirRef_RequestType_enum_t)INT32_C(1))
#define AirRef_RequestType_RequestMachineState ((AirRef_RequestType_enum_t)INT32_C(2))
#define AirRef_RequestType_RequestLoggerState ((AirRef_RequestType_enum_t)INT32_C(3))
#define AirRef_RequestType_AirRefConfReceived ((AirRef_RequestType_enum_t)INT32_C(4))

static inline const char *AirRef_RequestType_name(AirRef_RequestType_enum_t value)
{
    switch (value) {
    case AirRef_RequestType_RequestAirRefConf: return "RequestAirRefConf";
    case AirRef_RequestType_RequestAirRefState: return "RequestAirRefState";
    case AirRef_RequestType_RequestMachineState: return "RequestMachineState";
    case AirRef_RequestType_RequestLoggerState: return "RequestLoggerState";
    case AirRef_RequestType_AirRefConfReceived: return "AirRefConfReceived";
    default: return "";
    }
}

static inline int AirRef_RequestType_is_known_value(AirRef_RequestType_enum_t value)
{
    switch (value) {
    case AirRef_RequestType_RequestAirRefConf: return 1;
    case AirRef_RequestType_RequestAirRefState: return 1;
    case AirRef_RequestType_RequestMachineState: return 1;
    case AirRef_RequestType_RequestLoggerState: return 1;
    case AirRef_RequestType_AirRefConfReceived: return 1;
    default: return 0;
    }
}

typedef int32_t AirRef_LoggerCommunicationState_enum_t;
__flatbuffers_define_integer_type(AirRef_LoggerCommunicationState, AirRef_LoggerCommunicationState_enum_t, 32)
#define AirRef_LoggerCommunicationState_CommunicationIdle ((AirRef_LoggerCommunicationState_enum_t)INT32_C(0))
#define AirRef_LoggerCommunicationState_ComunicationDone ((AirRef_LoggerCommunicationState_enum_t)INT32_C(1))
#define AirRef_LoggerCommunicationState_CommunicationFailed ((AirRef_LoggerCommunicationState_enum_t)INT32_C(2))

static inline const char *AirRef_LoggerCommunicationState_name(AirRef_LoggerCommunicationState_enum_t value)
{
    switch (value) {
    case AirRef_LoggerCommunicationState_CommunicationIdle: return "CommunicationIdle";
    case AirRef_LoggerCommunicationState_ComunicationDone: return "ComunicationDone";
    case AirRef_LoggerCommunicationState_CommunicationFailed: return "CommunicationFailed";
    default: return "";
    }
}

static inline int AirRef_LoggerCommunicationState_is_known_value(AirRef_LoggerCommunicationState_enum_t value)
{
    switch (value) {
    case AirRef_LoggerCommunicationState_CommunicationIdle: return 1;
    case AirRef_LoggerCommunicationState_ComunicationDone: return 1;
    case AirRef_LoggerCommunicationState_CommunicationFailed: return 1;
    default: return 0;
    }
}


struct AirRef_ErrorReport {
    alignas(4) AirRef_ErrorState_enum_t error_state;
    alignas(4) int32_t error_code;
};
static_assert(sizeof(AirRef_ErrorReport_t) == 8, "struct size mismatch");

static inline const AirRef_ErrorReport_t *AirRef_ErrorReport__const_ptr_add(const AirRef_ErrorReport_t *p, size_t i) { return p + i; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport__ptr_add(AirRef_ErrorReport_t *p, size_t i) { return p + i; }
static inline AirRef_ErrorReport_struct_t AirRef_ErrorReport_vec_at(AirRef_ErrorReport_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t AirRef_ErrorReport__size(void) { return 8; }
static inline size_t AirRef_ErrorReport_vec_len(AirRef_ErrorReport_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(AirRef_ErrorReport)

__flatbuffers_define_struct_scalar_field(AirRef_ErrorReport, error_state, AirRef_ErrorState, AirRef_ErrorState_enum_t)
__flatbuffers_define_struct_scalar_field(AirRef_ErrorReport, error_code, flatbuffers_int32, int32_t)

struct AirRef_ErrorList {
    alignas(4) AirRef_ErrorReport_t errors[10];
};
static_assert(sizeof(AirRef_ErrorList_t) == 80, "struct size mismatch");

static inline const AirRef_ErrorList_t *AirRef_ErrorList__const_ptr_add(const AirRef_ErrorList_t *p, size_t i) { return p + i; }
static inline AirRef_ErrorList_t *AirRef_ErrorList__ptr_add(AirRef_ErrorList_t *p, size_t i) { return p + i; }
static inline AirRef_ErrorList_struct_t AirRef_ErrorList_vec_at(AirRef_ErrorList_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t AirRef_ErrorList__size(void) { return 80; }
static inline size_t AirRef_ErrorList_vec_len(AirRef_ErrorList_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(AirRef_ErrorList)

__flatbuffers_define_struct_struct_fixed_array_field(AirRef_ErrorList, errors, AirRef_ErrorReport_struct_t, 10)

struct AirRef_MotorStatus {
    alignas(4) int32_t state[10];
};
static_assert(sizeof(AirRef_MotorStatus_t) == 40, "struct size mismatch");

static inline const AirRef_MotorStatus_t *AirRef_MotorStatus__const_ptr_add(const AirRef_MotorStatus_t *p, size_t i) { return p + i; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus__ptr_add(AirRef_MotorStatus_t *p, size_t i) { return p + i; }
static inline AirRef_MotorStatus_struct_t AirRef_MotorStatus_vec_at(AirRef_MotorStatus_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t AirRef_MotorStatus__size(void) { return 40; }
static inline size_t AirRef_MotorStatus_vec_len(AirRef_MotorStatus_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(AirRef_MotorStatus)

__flatbuffers_define_struct_scalar_fixed_array_field(AirRef_MotorStatus, state, flatbuffers_int32, int32_t, 10)


struct AirRef_AirRefConf_table { uint8_t unused__; };

static inline size_t AirRef_AirRefConf_vec_len(AirRef_AirRefConf_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_AirRefConf_table_t AirRef_AirRefConf_vec_at(AirRef_AirRefConf_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_AirRefConf_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_AirRefConf)

__flatbuffers_define_scalar_field(0, AirRef_AirRefConf, control_type, AirRef_ControlType, AirRef_ControlType_enum_t, INT32_C(0))
__flatbuffers_define_scalar_field(1, AirRef_AirRefConf, termostatica_surriscaldo_setpoint, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(2, AirRef_AirRefConf, termostatica_coeff_P, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(3, AirRef_AirRefConf, termostatica_coeff_I, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(4, AirRef_AirRefConf, termostatica_coeff_I_max, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(5, AirRef_AirRefConf, termostatica_max_step, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(6, AirRef_AirRefConf, fan_target_pressure, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(7, AirRef_AirRefConf, fan_coeff_p, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(8, AirRef_AirRefConf, fan_coeff_offset, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(9, AirRef_AirRefConf, fan_min_pressure, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(10, AirRef_AirRefConf, fan_max_pressure, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(11, AirRef_AirRefConf, compressor_target_pressure, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(12, AirRef_AirRefConf, compressor_coeff_P, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(13, AirRef_AirRefConf, compressor_coeff_I, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(14, AirRef_AirRefConf, compressor_start_interval, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(15, AirRef_AirRefConf, low_pressure_limit, flatbuffers_int32, int32_t, INT32_C(-1))

struct AirRef_AirRefState_table { uint8_t unused__; };

static inline size_t AirRef_AirRefState_vec_len(AirRef_AirRefState_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_AirRefState_table_t AirRef_AirRefState_vec_at(AirRef_AirRefState_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_AirRefState_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_AirRefState)

__flatbuffers_define_scalar_field(0, AirRef_AirRefState, compressor_calculated_speed, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(1, AirRef_AirRefState, compressor_speed_to_command, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(2, AirRef_AirRefState, compressor_I_value, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(3, AirRef_AirRefState, compressor_last_stop, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(4, AirRef_AirRefState, compressor_is_blocked, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(5, AirRef_AirRefState, compressor_is_running, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(6, AirRef_AirRefState, fan_speed_to_command, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(7, AirRef_AirRefState, fan_time_last_command, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(8, AirRef_AirRefState, termostatica_I_value, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(9, AirRef_AirRefState, termostatica_step_target, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_scalar_field(10, AirRef_AirRefState, termostatica_step_current_position, flatbuffers_int32, int32_t, INT32_C(-1))

struct AirRef_MachineState_table { uint8_t unused__; };

static inline size_t AirRef_MachineState_vec_len(AirRef_MachineState_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_MachineState_table_t AirRef_MachineState_vec_at(AirRef_MachineState_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_MachineState_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_MachineState)

__flatbuffers_define_scalar_field(0, AirRef_MachineState, evaporation_pressure, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(1, AirRef_MachineState, evaporation_temperature, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(2, AirRef_MachineState, condensation_pressure, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(3, AirRef_MachineState, temperature_gas_scarico, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(4, AirRef_MachineState, temperature_environment, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(5, AirRef_MachineState, temperature_gas_ritorno, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_scalar_field(6, AirRef_MachineState, temperature_extra, flatbuffers_int32, int32_t, INT32_C(0))
__flatbuffers_define_struct_field(7, AirRef_MachineState, imc102_communication, AirRef_ErrorReport_struct_t, 0)
__flatbuffers_define_struct_field(8, AirRef_MachineState, imc102_motor, AirRef_ErrorReport_struct_t, 0)
__flatbuffers_define_struct_field(9, AirRef_MachineState, imc102_status, AirRef_MotorStatus_struct_t, 0)
__flatbuffers_define_struct_field(10, AirRef_MachineState, imm101_communication, AirRef_ErrorReport_struct_t, 0)
__flatbuffers_define_struct_field(11, AirRef_MachineState, imm101_motor, AirRef_ErrorReport_struct_t, 0)
__flatbuffers_define_struct_field(12, AirRef_MachineState, imm101_status, AirRef_MotorStatus_struct_t, 0)
__flatbuffers_define_scalar_field(13, AirRef_MachineState, pin_enable, flatbuffers_int32, int32_t, INT32_C(-1))
__flatbuffers_define_struct_field(14, AirRef_MachineState, ar_error, AirRef_ErrorList_struct_t, 0)
__flatbuffers_define_scalar_field(15, AirRef_MachineState, ar_status, AirRef_AirRefStatus, AirRef_AirRefStatus_enum_t, INT32_C(0))

struct AirRef_Request_table { uint8_t unused__; };

static inline size_t AirRef_Request_vec_len(AirRef_Request_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_Request_table_t AirRef_Request_vec_at(AirRef_Request_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_Request_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_Request)

__flatbuffers_define_scalar_field(0, AirRef_Request, request_type, AirRef_RequestType, AirRef_RequestType_enum_t, INT32_C(0))

struct AirRef_LoggerState_table { uint8_t unused__; };

static inline size_t AirRef_LoggerState_vec_len(AirRef_LoggerState_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_LoggerState_table_t AirRef_LoggerState_vec_at(AirRef_LoggerState_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_LoggerState_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_LoggerState)

__flatbuffers_define_scalar_field(0, AirRef_LoggerState, state, AirRef_LoggerCommunicationState, AirRef_LoggerCommunicationState_enum_t, INT32_C(0))
typedef uint8_t AirRef_Content_union_type_t;
__flatbuffers_define_integer_type(AirRef_Content, AirRef_Content_union_type_t, 8)
__flatbuffers_define_union(flatbuffers_, AirRef_Content)
#define AirRef_Content_NONE ((AirRef_Content_union_type_t)UINT8_C(0))
#define AirRef_Content_AirRefConf ((AirRef_Content_union_type_t)UINT8_C(1))
#define AirRef_Content_AirRefState ((AirRef_Content_union_type_t)UINT8_C(2))
#define AirRef_Content_MachineState ((AirRef_Content_union_type_t)UINT8_C(3))
#define AirRef_Content_Request ((AirRef_Content_union_type_t)UINT8_C(4))
#define AirRef_Content_BluefiCommand ((AirRef_Content_union_type_t)UINT8_C(5))
#define AirRef_Content_BluefiReply ((AirRef_Content_union_type_t)UINT8_C(6))
#define AirRef_Content_LoggerCommand ((AirRef_Content_union_type_t)UINT8_C(7))
#define AirRef_Content_LoggerReply ((AirRef_Content_union_type_t)UINT8_C(8))

static inline const char *AirRef_Content_type_name(AirRef_Content_union_type_t type)
{
    switch (type) {
    case AirRef_Content_NONE: return "NONE";
    case AirRef_Content_AirRefConf: return "AirRefConf";
    case AirRef_Content_AirRefState: return "AirRefState";
    case AirRef_Content_MachineState: return "MachineState";
    case AirRef_Content_Request: return "Request";
    case AirRef_Content_BluefiCommand: return "BluefiCommand";
    case AirRef_Content_BluefiReply: return "BluefiReply";
    case AirRef_Content_LoggerCommand: return "LoggerCommand";
    case AirRef_Content_LoggerReply: return "LoggerReply";
    default: return "";
    }
}

static inline int AirRef_Content_is_known_type(AirRef_Content_union_type_t type)
{
    switch (type) {
    case AirRef_Content_NONE: return 1;
    case AirRef_Content_AirRefConf: return 1;
    case AirRef_Content_AirRefState: return 1;
    case AirRef_Content_MachineState: return 1;
    case AirRef_Content_Request: return 1;
    case AirRef_Content_BluefiCommand: return 1;
    case AirRef_Content_BluefiReply: return 1;
    case AirRef_Content_LoggerCommand: return 1;
    case AirRef_Content_LoggerReply: return 1;
    default: return 0;
    }
}


struct AirRef_Message_table { uint8_t unused__; };

static inline size_t AirRef_Message_vec_len(AirRef_Message_vec_t vec)
__flatbuffers_vec_len(vec)
static inline AirRef_Message_table_t AirRef_Message_vec_at(AirRef_Message_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(AirRef_Message_table_t, vec, i, 0)
__flatbuffers_table_as_root(AirRef_Message)

__flatbuffers_define_union_field(flatbuffers_, 1, AirRef_Message, content, AirRef_Content, 0)


#include "flatcc/flatcc_epilogue.h"
#endif /* AIRREF_READER_H */
