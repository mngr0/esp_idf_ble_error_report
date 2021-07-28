#ifndef AIRREF_VERIFIER_H
#define AIRREF_VERIFIER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef AIRREF_READER_H
#include "airref_reader.h"
#endif
#include "flatcc/flatcc_verifier.h"
#include "flatcc/flatcc_prologue.h"

static int AirRef_AirRefConf_verify_table(flatcc_table_verifier_descriptor_t *td);
static int AirRef_AirRefState_verify_table(flatcc_table_verifier_descriptor_t *td);
static int AirRef_MachineState_verify_table(flatcc_table_verifier_descriptor_t *td);
static int AirRef_Request_verify_table(flatcc_table_verifier_descriptor_t *td);
static int AirRef_Message_verify_table(flatcc_table_verifier_descriptor_t *td);

static int AirRef_Content_union_verifier(flatcc_union_verifier_descriptor_t *ud)
{
    switch (ud->type) {
    case 1: return flatcc_verify_union_table(ud, AirRef_AirRefConf_verify_table); /* AirRefConf */
    case 2: return flatcc_verify_union_table(ud, AirRef_AirRefState_verify_table); /* AirRefState */
    case 3: return flatcc_verify_union_table(ud, AirRef_MachineState_verify_table); /* MachineState */
    case 4: return flatcc_verify_union_table(ud, AirRef_Request_verify_table); /* Request */
    default: return flatcc_verify_ok;
    }
}

static inline int AirRef_ErrorReport_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, AirRef_ErrorReport_identifier, 8, 4);
}

static inline int AirRef_ErrorReport_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, AirRef_ErrorReport_type_hash, 8, 4);
}

static inline int AirRef_ErrorReport_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, thash, 8, 4);
}

static inline int AirRef_ErrorReport_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, fid, 8, 4);
}

static inline int AirRef_CompressorSpeed_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, AirRef_CompressorSpeed_identifier, 40, 4);
}

static inline int AirRef_CompressorSpeed_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, AirRef_CompressorSpeed_type_hash, 40, 4);
}

static inline int AirRef_CompressorSpeed_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, thash, 40, 4);
}

static inline int AirRef_CompressorSpeed_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, fid, 40, 4);
}

static inline int AirRef_ErrorList_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, AirRef_ErrorList_identifier, 80, 4);
}

static inline int AirRef_ErrorList_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, AirRef_ErrorList_type_hash, 80, 4);
}

static inline int AirRef_ErrorList_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, thash, 80, 4);
}

static inline int AirRef_ErrorList_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, fid, 80, 4);
}

static inline int AirRef_MotorStatus_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, AirRef_MotorStatus_identifier, 40, 4);
}

static inline int AirRef_MotorStatus_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, AirRef_MotorStatus_type_hash, 40, 4);
}

static inline int AirRef_MotorStatus_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_struct_as_typed_root(buf, bufsiz, thash, 40, 4);
}

static inline int AirRef_MotorStatus_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_struct_as_root(buf, bufsiz, fid, 40, 4);
}

static int AirRef_AirRefConf_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_field(td, 0, 4, 4) /* serial_control */)) return ret;
    if ((ret = flatcc_verify_field(td, 1, 4, 4) /* fan_target_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 2, 4, 4) /* fan_coeff_offset */)) return ret;
    if ((ret = flatcc_verify_field(td, 3, 4, 4) /* fan_min_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 4, 4, 4) /* fan_max_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 5, 4, 4) /* compressor_target_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 6, 4, 4) /* compressor_activation_offset */)) return ret;
    if ((ret = flatcc_verify_field(td, 7, 4, 4) /* compressor_action_delay */)) return ret;
    if ((ret = flatcc_verify_field(td, 8, 40, 4) /* compressor_speed */)) return ret;
    if ((ret = flatcc_verify_field(td, 9, 4, 4) /* compressor_start_interval */)) return ret;
    return flatcc_verify_ok;
}

static inline int AirRef_AirRefConf_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_AirRefConf_identifier, &AirRef_AirRefConf_verify_table);
}

static inline int AirRef_AirRefConf_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_AirRefConf_type_identifier, &AirRef_AirRefConf_verify_table);
}

static inline int AirRef_AirRefConf_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &AirRef_AirRefConf_verify_table);
}

static inline int AirRef_AirRefConf_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &AirRef_AirRefConf_verify_table);
}

static int AirRef_AirRefState_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_field(td, 0, 4, 4) /* compressor_actual_speed_index */)) return ret;
    if ((ret = flatcc_verify_field(td, 1, 4, 4) /* compressor_last_speed_change_time */)) return ret;
    if ((ret = flatcc_verify_field(td, 2, 4, 4) /* compressor_speed_to_command */)) return ret;
    if ((ret = flatcc_verify_field(td, 3, 4, 4) /* compressor_last_stop */)) return ret;
    if ((ret = flatcc_verify_field(td, 4, 4, 4) /* compressor_is_blocked */)) return ret;
    if ((ret = flatcc_verify_field(td, 5, 4, 4) /* compressor_is_running */)) return ret;
    if ((ret = flatcc_verify_field(td, 6, 4, 4) /* fan_speed_to_command */)) return ret;
    if ((ret = flatcc_verify_field(td, 7, 4, 4) /* fan_time_last_command */)) return ret;
    return flatcc_verify_ok;
}

static inline int AirRef_AirRefState_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_AirRefState_identifier, &AirRef_AirRefState_verify_table);
}

static inline int AirRef_AirRefState_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_AirRefState_type_identifier, &AirRef_AirRefState_verify_table);
}

static inline int AirRef_AirRefState_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &AirRef_AirRefState_verify_table);
}

static inline int AirRef_AirRefState_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &AirRef_AirRefState_verify_table);
}

static int AirRef_MachineState_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_field(td, 0, 4, 4) /* evaporation_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 1, 4, 4) /* condensation_pressure */)) return ret;
    if ((ret = flatcc_verify_field(td, 2, 4, 4) /* temperature_gas_scarico */)) return ret;
    if ((ret = flatcc_verify_field(td, 3, 4, 4) /* temperature_environment */)) return ret;
    if ((ret = flatcc_verify_field(td, 4, 4, 4) /* temperature_gas_ritorno */)) return ret;
    if ((ret = flatcc_verify_field(td, 5, 4, 4) /* temperature_extra */)) return ret;
    if ((ret = flatcc_verify_field(td, 6, 8, 4) /* imc102_communication */)) return ret;
    if ((ret = flatcc_verify_field(td, 7, 8, 4) /* imc102_motor */)) return ret;
    if ((ret = flatcc_verify_field(td, 8, 40, 4) /* imc102_status */)) return ret;
    if ((ret = flatcc_verify_field(td, 9, 8, 4) /* imm101_communication */)) return ret;
    if ((ret = flatcc_verify_field(td, 10, 8, 4) /* imm101_motor */)) return ret;
    if ((ret = flatcc_verify_field(td, 11, 40, 4) /* imm101_status */)) return ret;
    if ((ret = flatcc_verify_field(td, 12, 4, 4) /* pin_enable */)) return ret;
    if ((ret = flatcc_verify_field(td, 13, 80, 4) /* ar_error */)) return ret;
    if ((ret = flatcc_verify_field(td, 14, 8, 4) /* ar_status */)) return ret;
    return flatcc_verify_ok;
}

static inline int AirRef_MachineState_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_MachineState_identifier, &AirRef_MachineState_verify_table);
}

static inline int AirRef_MachineState_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_MachineState_type_identifier, &AirRef_MachineState_verify_table);
}

static inline int AirRef_MachineState_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &AirRef_MachineState_verify_table);
}

static inline int AirRef_MachineState_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &AirRef_MachineState_verify_table);
}

static int AirRef_Request_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_field(td, 0, 4, 4) /* requestType */)) return ret;
    return flatcc_verify_ok;
}

static inline int AirRef_Request_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_Request_identifier, &AirRef_Request_verify_table);
}

static inline int AirRef_Request_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_Request_type_identifier, &AirRef_Request_verify_table);
}

static inline int AirRef_Request_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &AirRef_Request_verify_table);
}

static inline int AirRef_Request_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &AirRef_Request_verify_table);
}

static int AirRef_Message_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_union_field(td, 1, 0, &AirRef_Content_union_verifier) /* content */)) return ret;
    return flatcc_verify_ok;
}

static inline int AirRef_Message_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_Message_identifier, &AirRef_Message_verify_table);
}

static inline int AirRef_Message_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, AirRef_Message_type_identifier, &AirRef_Message_verify_table);
}

static inline int AirRef_Message_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &AirRef_Message_verify_table);
}

static inline int AirRef_Message_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &AirRef_Message_verify_table);
}

#include "flatcc/flatcc_epilogue.h"
#endif /* AIRREF_VERIFIER_H */
