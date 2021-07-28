#ifndef AIRREF_BUILDER_H
#define AIRREF_BUILDER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef AIRREF_READER_H
#include "airref_reader.h"
#endif
#ifndef FLATBUFFERS_COMMON_BUILDER_H
#include "flatbuffers_common_builder.h"
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif

#define __AirRef_ErrorState_formal_args , AirRef_ErrorState_enum_t v0
#define __AirRef_ErrorState_call_args , v0
__flatbuffers_build_scalar(flatbuffers_, AirRef_ErrorState, AirRef_ErrorState_enum_t)
#define __AirRef_AirRefIndexError_formal_args , AirRef_AirRefIndexError_enum_t v0
#define __AirRef_AirRefIndexError_call_args , v0
__flatbuffers_build_scalar(flatbuffers_, AirRef_AirRefIndexError, AirRef_AirRefIndexError_enum_t)
#define __AirRef_AirRefStatus_formal_args , AirRef_AirRefStatus_enum_t v0
#define __AirRef_AirRefStatus_call_args , v0
__flatbuffers_build_scalar(flatbuffers_, AirRef_AirRefStatus, AirRef_AirRefStatus_enum_t)
#define __AirRef_RequestType_formal_args , AirRef_RequestType_enum_t v0
#define __AirRef_RequestType_call_args , v0
__flatbuffers_build_scalar(flatbuffers_, AirRef_RequestType, AirRef_RequestType_enum_t)

#define __AirRef_ErrorReport_formal_args , AirRef_ErrorState_enum_t v0, int32_t v1
#define __AirRef_ErrorReport_call_args , v0, v1
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_assign(AirRef_ErrorReport_t *p, AirRef_ErrorState_enum_t v0, int32_t v1)
{ p->error_state = v0; p->error_code = v1;
  return p; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_copy(AirRef_ErrorReport_t *p, const AirRef_ErrorReport_t *p2)
{ p->error_state = p2->error_state; p->error_code = p2->error_code;
  return p; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_assign_to_pe(AirRef_ErrorReport_t *p, AirRef_ErrorState_enum_t v0, int32_t v1)
{ AirRef_ErrorState_assign_to_pe(&p->error_state, v0); flatbuffers_int32_assign_to_pe(&p->error_code, v1);
  return p; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_copy_to_pe(AirRef_ErrorReport_t *p, const AirRef_ErrorReport_t *p2)
{ AirRef_ErrorState_copy_to_pe(&p->error_state, &p2->error_state); flatbuffers_int32_copy_to_pe(&p->error_code, &p2->error_code);
  return p; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_assign_from_pe(AirRef_ErrorReport_t *p, AirRef_ErrorState_enum_t v0, int32_t v1)
{ AirRef_ErrorState_assign_from_pe(&p->error_state, v0); flatbuffers_int32_assign_from_pe(&p->error_code, v1);
  return p; }
static inline AirRef_ErrorReport_t *AirRef_ErrorReport_copy_from_pe(AirRef_ErrorReport_t *p, const AirRef_ErrorReport_t *p2)
{ AirRef_ErrorState_copy_from_pe(&p->error_state, &p2->error_state); flatbuffers_int32_copy_from_pe(&p->error_code, &p2->error_code);
  return p; }
__flatbuffers_build_struct(flatbuffers_, AirRef_ErrorReport, 8, 4, AirRef_ErrorReport_file_identifier, AirRef_ErrorReport_type_identifier)
__flatbuffers_define_fixed_array_primitives(flatbuffers_, AirRef_ErrorReport, AirRef_ErrorReport_t)

#define __AirRef_CompressorSpeed_formal_args , const int32_t v0[10]
#define __AirRef_CompressorSpeed_call_args , v0
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_assign(AirRef_CompressorSpeed_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy(p->speed, v0, 10);
  return p; }
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_copy(AirRef_CompressorSpeed_t *p, const AirRef_CompressorSpeed_t *p2)
{ flatbuffers_int32_array_copy(p->speed, p2->speed, 10);
  return p; }
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_assign_to_pe(AirRef_CompressorSpeed_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy_to_pe(p->speed, v0, 10);
  return p; }
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_copy_to_pe(AirRef_CompressorSpeed_t *p, const AirRef_CompressorSpeed_t *p2)
{ flatbuffers_int32_array_copy_to_pe(p->speed, p2->speed, 10);
  return p; }
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_assign_from_pe(AirRef_CompressorSpeed_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy_from_pe(p->speed, v0, 10);
  return p; }
static inline AirRef_CompressorSpeed_t *AirRef_CompressorSpeed_copy_from_pe(AirRef_CompressorSpeed_t *p, const AirRef_CompressorSpeed_t *p2)
{ flatbuffers_int32_array_copy_from_pe(p->speed, p2->speed, 10);
  return p; }
__flatbuffers_build_struct(flatbuffers_, AirRef_CompressorSpeed, 40, 4, AirRef_CompressorSpeed_file_identifier, AirRef_CompressorSpeed_type_identifier)
__flatbuffers_define_fixed_array_primitives(flatbuffers_, AirRef_CompressorSpeed, AirRef_CompressorSpeed_t)

#define __AirRef_ErrorList_formal_args , const AirRef_ErrorReport_t v0[10]
#define __AirRef_ErrorList_call_args , v0
static inline AirRef_ErrorList_t *AirRef_ErrorList_assign(AirRef_ErrorList_t *p, const AirRef_ErrorReport_t v0[10])
{ AirRef_ErrorReport_array_copy(p->speed, v0, 10);
  return p; }
static inline AirRef_ErrorList_t *AirRef_ErrorList_copy(AirRef_ErrorList_t *p, const AirRef_ErrorList_t *p2)
{ AirRef_ErrorReport_array_copy(p->speed, p2->speed, 10);
  return p; }
static inline AirRef_ErrorList_t *AirRef_ErrorList_assign_to_pe(AirRef_ErrorList_t *p, const AirRef_ErrorReport_t v0[10])
{ AirRef_ErrorReport_array_copy_to_pe(p->speed, v0, 10);
  return p; }
static inline AirRef_ErrorList_t *AirRef_ErrorList_copy_to_pe(AirRef_ErrorList_t *p, const AirRef_ErrorList_t *p2)
{ AirRef_ErrorReport_array_copy_to_pe(p->speed, p2->speed, 10);
  return p; }
static inline AirRef_ErrorList_t *AirRef_ErrorList_assign_from_pe(AirRef_ErrorList_t *p, const AirRef_ErrorReport_t v0[10])
{ AirRef_ErrorReport_array_copy_from_pe(p->speed, v0, 10);
  return p; }
static inline AirRef_ErrorList_t *AirRef_ErrorList_copy_from_pe(AirRef_ErrorList_t *p, const AirRef_ErrorList_t *p2)
{ AirRef_ErrorReport_array_copy_from_pe(p->speed, p2->speed, 10);
  return p; }
__flatbuffers_build_struct(flatbuffers_, AirRef_ErrorList, 80, 4, AirRef_ErrorList_file_identifier, AirRef_ErrorList_type_identifier)
__flatbuffers_define_fixed_array_primitives(flatbuffers_, AirRef_ErrorList, AirRef_ErrorList_t)

#define __AirRef_MotorStatus_formal_args , const int32_t v0[10]
#define __AirRef_MotorStatus_call_args , v0
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_assign(AirRef_MotorStatus_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy(p->state, v0, 10);
  return p; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_copy(AirRef_MotorStatus_t *p, const AirRef_MotorStatus_t *p2)
{ flatbuffers_int32_array_copy(p->state, p2->state, 10);
  return p; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_assign_to_pe(AirRef_MotorStatus_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy_to_pe(p->state, v0, 10);
  return p; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_copy_to_pe(AirRef_MotorStatus_t *p, const AirRef_MotorStatus_t *p2)
{ flatbuffers_int32_array_copy_to_pe(p->state, p2->state, 10);
  return p; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_assign_from_pe(AirRef_MotorStatus_t *p, const int32_t v0[10])
{ flatbuffers_int32_array_copy_from_pe(p->state, v0, 10);
  return p; }
static inline AirRef_MotorStatus_t *AirRef_MotorStatus_copy_from_pe(AirRef_MotorStatus_t *p, const AirRef_MotorStatus_t *p2)
{ flatbuffers_int32_array_copy_from_pe(p->state, p2->state, 10);
  return p; }
__flatbuffers_build_struct(flatbuffers_, AirRef_MotorStatus, 40, 4, AirRef_MotorStatus_file_identifier, AirRef_MotorStatus_type_identifier)
__flatbuffers_define_fixed_array_primitives(flatbuffers_, AirRef_MotorStatus, AirRef_MotorStatus_t)

typedef flatbuffers_union_ref_t AirRef_Content_union_ref_t;
typedef flatbuffers_union_vec_ref_t AirRef_Content_union_vec_ref_t;
static AirRef_Content_union_ref_t AirRef_Content_clone(flatbuffers_builder_t *B, AirRef_Content_union_t t);

static const flatbuffers_voffset_t __AirRef_AirRefConf_required[] = { 0 };
typedef flatbuffers_ref_t AirRef_AirRefConf_ref_t;
static AirRef_AirRefConf_ref_t AirRef_AirRefConf_clone(flatbuffers_builder_t *B, AirRef_AirRefConf_table_t t);
__flatbuffers_build_table(flatbuffers_, AirRef_AirRefConf, 10)

static const flatbuffers_voffset_t __AirRef_AirRefState_required[] = { 0 };
typedef flatbuffers_ref_t AirRef_AirRefState_ref_t;
static AirRef_AirRefState_ref_t AirRef_AirRefState_clone(flatbuffers_builder_t *B, AirRef_AirRefState_table_t t);
__flatbuffers_build_table(flatbuffers_, AirRef_AirRefState, 8)

static const flatbuffers_voffset_t __AirRef_MachineState_required[] = { 0 };
typedef flatbuffers_ref_t AirRef_MachineState_ref_t;
static AirRef_MachineState_ref_t AirRef_MachineState_clone(flatbuffers_builder_t *B, AirRef_MachineState_table_t t);
__flatbuffers_build_table(flatbuffers_, AirRef_MachineState, 15)

static const flatbuffers_voffset_t __AirRef_Request_required[] = { 0 };
typedef flatbuffers_ref_t AirRef_Request_ref_t;
static AirRef_Request_ref_t AirRef_Request_clone(flatbuffers_builder_t *B, AirRef_Request_table_t t);
__flatbuffers_build_table(flatbuffers_, AirRef_Request, 1)

static const flatbuffers_voffset_t __AirRef_Message_required[] = { 0 };
typedef flatbuffers_ref_t AirRef_Message_ref_t;
static AirRef_Message_ref_t AirRef_Message_clone(flatbuffers_builder_t *B, AirRef_Message_table_t t);
__flatbuffers_build_table(flatbuffers_, AirRef_Message, 2)

#define __AirRef_AirRefConf_formal_args ,\
  int32_t v0, int32_t v1, int32_t v2, int32_t v3,\
  int32_t v4, int32_t v5, int32_t v6, int32_t v7, AirRef_CompressorSpeed_t *v8, int32_t v9
#define __AirRef_AirRefConf_call_args ,\
  v0, v1, v2, v3,\
  v4, v5, v6, v7, v8, v9
static inline AirRef_AirRefConf_ref_t AirRef_AirRefConf_create(flatbuffers_builder_t *B __AirRef_AirRefConf_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, AirRef_AirRefConf, AirRef_AirRefConf_file_identifier, AirRef_AirRefConf_type_identifier)

#define __AirRef_AirRefState_formal_args ,\
  int32_t v0, int32_t v1, int32_t v2, int32_t v3,\
  int32_t v4, int32_t v5, int32_t v6, int32_t v7
#define __AirRef_AirRefState_call_args ,\
  v0, v1, v2, v3,\
  v4, v5, v6, v7
static inline AirRef_AirRefState_ref_t AirRef_AirRefState_create(flatbuffers_builder_t *B __AirRef_AirRefState_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, AirRef_AirRefState, AirRef_AirRefState_file_identifier, AirRef_AirRefState_type_identifier)

#define __AirRef_MachineState_formal_args ,\
  int32_t v0, int32_t v1, int32_t v2, int32_t v3,\
  int32_t v4, int32_t v5, AirRef_ErrorReport_t *v6, AirRef_ErrorReport_t *v7,\
  AirRef_MotorStatus_t *v8, AirRef_ErrorReport_t *v9, AirRef_ErrorReport_t *v10, AirRef_MotorStatus_t *v11,\
  int32_t v12, AirRef_ErrorList_t *v13, AirRef_ErrorReport_t *v14
#define __AirRef_MachineState_call_args ,\
  v0, v1, v2, v3,\
  v4, v5, v6, v7,\
  v8, v9, v10, v11,\
  v12, v13, v14
static inline AirRef_MachineState_ref_t AirRef_MachineState_create(flatbuffers_builder_t *B __AirRef_MachineState_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, AirRef_MachineState, AirRef_MachineState_file_identifier, AirRef_MachineState_type_identifier)

#define __AirRef_Request_formal_args , AirRef_RequestType_enum_t v0
#define __AirRef_Request_call_args , v0
static inline AirRef_Request_ref_t AirRef_Request_create(flatbuffers_builder_t *B __AirRef_Request_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, AirRef_Request, AirRef_Request_file_identifier, AirRef_Request_type_identifier)

#define __AirRef_Message_formal_args , AirRef_Content_union_ref_t v1
#define __AirRef_Message_call_args , v1
static inline AirRef_Message_ref_t AirRef_Message_create(flatbuffers_builder_t *B __AirRef_Message_formal_args);
__flatbuffers_build_table_prolog(flatbuffers_, AirRef_Message, AirRef_Message_file_identifier, AirRef_Message_type_identifier)

static inline AirRef_Content_union_ref_t AirRef_Content_as_NONE(void)
{ AirRef_Content_union_ref_t uref; uref.type = AirRef_Content_NONE; uref.value = 0; return uref; }
static inline AirRef_Content_union_ref_t AirRef_Content_as_AirRefConf(AirRef_AirRefConf_ref_t ref)
{ AirRef_Content_union_ref_t uref; uref.type = AirRef_Content_AirRefConf; uref.value = ref; return uref; }
static inline AirRef_Content_union_ref_t AirRef_Content_as_AirRefState(AirRef_AirRefState_ref_t ref)
{ AirRef_Content_union_ref_t uref; uref.type = AirRef_Content_AirRefState; uref.value = ref; return uref; }
static inline AirRef_Content_union_ref_t AirRef_Content_as_MachineState(AirRef_MachineState_ref_t ref)
{ AirRef_Content_union_ref_t uref; uref.type = AirRef_Content_MachineState; uref.value = ref; return uref; }
static inline AirRef_Content_union_ref_t AirRef_Content_as_Request(AirRef_Request_ref_t ref)
{ AirRef_Content_union_ref_t uref; uref.type = AirRef_Content_Request; uref.value = ref; return uref; }
__flatbuffers_build_union_vector(flatbuffers_, AirRef_Content)

static AirRef_Content_union_ref_t AirRef_Content_clone(flatbuffers_builder_t *B, AirRef_Content_union_t u)
{
    switch (u.type) {
    case 1: return AirRef_Content_as_AirRefConf(AirRef_AirRefConf_clone(B, (AirRef_AirRefConf_table_t)u.value));
    case 2: return AirRef_Content_as_AirRefState(AirRef_AirRefState_clone(B, (AirRef_AirRefState_table_t)u.value));
    case 3: return AirRef_Content_as_MachineState(AirRef_MachineState_clone(B, (AirRef_MachineState_table_t)u.value));
    case 4: return AirRef_Content_as_Request(AirRef_Request_clone(B, (AirRef_Request_table_t)u.value));
    default: return AirRef_Content_as_NONE();
    }
}

__flatbuffers_build_scalar_field(0, flatbuffers_, AirRef_AirRefConf_serial_control, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(1, flatbuffers_, AirRef_AirRefConf_fan_target_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(2, flatbuffers_, AirRef_AirRefConf_fan_coeff_offset, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(3, flatbuffers_, AirRef_AirRefConf_fan_min_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(4, flatbuffers_, AirRef_AirRefConf_fan_max_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(5, flatbuffers_, AirRef_AirRefConf_compressor_target_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(6, flatbuffers_, AirRef_AirRefConf_compressor_activation_offset, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_scalar_field(7, flatbuffers_, AirRef_AirRefConf_compressor_action_delay, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)
__flatbuffers_build_struct_field(8, flatbuffers_, AirRef_AirRefConf_compressor_speed, AirRef_CompressorSpeed, 40, 4, AirRef_AirRefConf)
__flatbuffers_build_scalar_field(9, flatbuffers_, AirRef_AirRefConf_compressor_start_interval, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefConf)

static inline AirRef_AirRefConf_ref_t AirRef_AirRefConf_create(flatbuffers_builder_t *B __AirRef_AirRefConf_formal_args)
{
    if (AirRef_AirRefConf_start(B)
        || AirRef_AirRefConf_serial_control_add(B, v0)
        || AirRef_AirRefConf_fan_target_pressure_add(B, v1)
        || AirRef_AirRefConf_fan_coeff_offset_add(B, v2)
        || AirRef_AirRefConf_fan_min_pressure_add(B, v3)
        || AirRef_AirRefConf_fan_max_pressure_add(B, v4)
        || AirRef_AirRefConf_compressor_target_pressure_add(B, v5)
        || AirRef_AirRefConf_compressor_activation_offset_add(B, v6)
        || AirRef_AirRefConf_compressor_action_delay_add(B, v7)
        || AirRef_AirRefConf_compressor_speed_add(B, v8)
        || AirRef_AirRefConf_compressor_start_interval_add(B, v9)) {
        return 0;
    }
    return AirRef_AirRefConf_end(B);
}

static AirRef_AirRefConf_ref_t AirRef_AirRefConf_clone(flatbuffers_builder_t *B, AirRef_AirRefConf_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (AirRef_AirRefConf_start(B)
        || AirRef_AirRefConf_serial_control_pick(B, t)
        || AirRef_AirRefConf_fan_target_pressure_pick(B, t)
        || AirRef_AirRefConf_fan_coeff_offset_pick(B, t)
        || AirRef_AirRefConf_fan_min_pressure_pick(B, t)
        || AirRef_AirRefConf_fan_max_pressure_pick(B, t)
        || AirRef_AirRefConf_compressor_target_pressure_pick(B, t)
        || AirRef_AirRefConf_compressor_activation_offset_pick(B, t)
        || AirRef_AirRefConf_compressor_action_delay_pick(B, t)
        || AirRef_AirRefConf_compressor_speed_pick(B, t)
        || AirRef_AirRefConf_compressor_start_interval_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, AirRef_AirRefConf_end(B));
}

__flatbuffers_build_scalar_field(0, flatbuffers_, AirRef_AirRefState_compressor_actual_speed_index, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(1, flatbuffers_, AirRef_AirRefState_compressor_last_speed_change_time, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(2, flatbuffers_, AirRef_AirRefState_compressor_speed_to_command, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(3, flatbuffers_, AirRef_AirRefState_compressor_last_stop, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(4, flatbuffers_, AirRef_AirRefState_compressor_is_blocked, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(5, flatbuffers_, AirRef_AirRefState_compressor_is_running, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(6, flatbuffers_, AirRef_AirRefState_fan_speed_to_command, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)
__flatbuffers_build_scalar_field(7, flatbuffers_, AirRef_AirRefState_fan_time_last_command, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_AirRefState)

static inline AirRef_AirRefState_ref_t AirRef_AirRefState_create(flatbuffers_builder_t *B __AirRef_AirRefState_formal_args)
{
    if (AirRef_AirRefState_start(B)
        || AirRef_AirRefState_compressor_actual_speed_index_add(B, v0)
        || AirRef_AirRefState_compressor_last_speed_change_time_add(B, v1)
        || AirRef_AirRefState_compressor_speed_to_command_add(B, v2)
        || AirRef_AirRefState_compressor_last_stop_add(B, v3)
        || AirRef_AirRefState_compressor_is_blocked_add(B, v4)
        || AirRef_AirRefState_compressor_is_running_add(B, v5)
        || AirRef_AirRefState_fan_speed_to_command_add(B, v6)
        || AirRef_AirRefState_fan_time_last_command_add(B, v7)) {
        return 0;
    }
    return AirRef_AirRefState_end(B);
}

static AirRef_AirRefState_ref_t AirRef_AirRefState_clone(flatbuffers_builder_t *B, AirRef_AirRefState_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (AirRef_AirRefState_start(B)
        || AirRef_AirRefState_compressor_actual_speed_index_pick(B, t)
        || AirRef_AirRefState_compressor_last_speed_change_time_pick(B, t)
        || AirRef_AirRefState_compressor_speed_to_command_pick(B, t)
        || AirRef_AirRefState_compressor_last_stop_pick(B, t)
        || AirRef_AirRefState_compressor_is_blocked_pick(B, t)
        || AirRef_AirRefState_compressor_is_running_pick(B, t)
        || AirRef_AirRefState_fan_speed_to_command_pick(B, t)
        || AirRef_AirRefState_fan_time_last_command_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, AirRef_AirRefState_end(B));
}

__flatbuffers_build_scalar_field(0, flatbuffers_, AirRef_MachineState_evaporation_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_scalar_field(1, flatbuffers_, AirRef_MachineState_condensation_pressure, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_scalar_field(2, flatbuffers_, AirRef_MachineState_temperature_gas_scarico, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_scalar_field(3, flatbuffers_, AirRef_MachineState_temperature_environment, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_scalar_field(4, flatbuffers_, AirRef_MachineState_temperature_gas_ritorno, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_scalar_field(5, flatbuffers_, AirRef_MachineState_temperature_extra, flatbuffers_int32, int32_t, 4, 4, INT32_C(0), AirRef_MachineState)
__flatbuffers_build_struct_field(6, flatbuffers_, AirRef_MachineState_imc102_communication, AirRef_ErrorReport, 8, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(7, flatbuffers_, AirRef_MachineState_imc102_motor, AirRef_ErrorReport, 8, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(8, flatbuffers_, AirRef_MachineState_imc102_status, AirRef_MotorStatus, 40, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(9, flatbuffers_, AirRef_MachineState_imm101_communication, AirRef_ErrorReport, 8, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(10, flatbuffers_, AirRef_MachineState_imm101_motor, AirRef_ErrorReport, 8, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(11, flatbuffers_, AirRef_MachineState_imm101_status, AirRef_MotorStatus, 40, 4, AirRef_MachineState)
__flatbuffers_build_scalar_field(12, flatbuffers_, AirRef_MachineState_pin_enable, flatbuffers_int32, int32_t, 4, 4, INT32_C(-1), AirRef_MachineState)
__flatbuffers_build_struct_field(13, flatbuffers_, AirRef_MachineState_ar_error, AirRef_ErrorList, 80, 4, AirRef_MachineState)
__flatbuffers_build_struct_field(14, flatbuffers_, AirRef_MachineState_ar_status, AirRef_ErrorReport, 8, 4, AirRef_MachineState)

static inline AirRef_MachineState_ref_t AirRef_MachineState_create(flatbuffers_builder_t *B __AirRef_MachineState_formal_args)
{
    if (AirRef_MachineState_start(B)
        || AirRef_MachineState_evaporation_pressure_add(B, v0)
        || AirRef_MachineState_condensation_pressure_add(B, v1)
        || AirRef_MachineState_temperature_gas_scarico_add(B, v2)
        || AirRef_MachineState_temperature_environment_add(B, v3)
        || AirRef_MachineState_temperature_gas_ritorno_add(B, v4)
        || AirRef_MachineState_temperature_extra_add(B, v5)
        || AirRef_MachineState_imc102_communication_add(B, v6)
        || AirRef_MachineState_imc102_motor_add(B, v7)
        || AirRef_MachineState_imc102_status_add(B, v8)
        || AirRef_MachineState_imm101_communication_add(B, v9)
        || AirRef_MachineState_imm101_motor_add(B, v10)
        || AirRef_MachineState_imm101_status_add(B, v11)
        || AirRef_MachineState_pin_enable_add(B, v12)
        || AirRef_MachineState_ar_error_add(B, v13)
        || AirRef_MachineState_ar_status_add(B, v14)) {
        return 0;
    }
    return AirRef_MachineState_end(B);
}

static AirRef_MachineState_ref_t AirRef_MachineState_clone(flatbuffers_builder_t *B, AirRef_MachineState_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (AirRef_MachineState_start(B)
        || AirRef_MachineState_evaporation_pressure_pick(B, t)
        || AirRef_MachineState_condensation_pressure_pick(B, t)
        || AirRef_MachineState_temperature_gas_scarico_pick(B, t)
        || AirRef_MachineState_temperature_environment_pick(B, t)
        || AirRef_MachineState_temperature_gas_ritorno_pick(B, t)
        || AirRef_MachineState_temperature_extra_pick(B, t)
        || AirRef_MachineState_imc102_communication_pick(B, t)
        || AirRef_MachineState_imc102_motor_pick(B, t)
        || AirRef_MachineState_imc102_status_pick(B, t)
        || AirRef_MachineState_imm101_communication_pick(B, t)
        || AirRef_MachineState_imm101_motor_pick(B, t)
        || AirRef_MachineState_imm101_status_pick(B, t)
        || AirRef_MachineState_pin_enable_pick(B, t)
        || AirRef_MachineState_ar_error_pick(B, t)
        || AirRef_MachineState_ar_status_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, AirRef_MachineState_end(B));
}

__flatbuffers_build_scalar_field(0, flatbuffers_, AirRef_Request_requestType, AirRef_RequestType, AirRef_RequestType_enum_t, 4, 4, INT32_C(0), AirRef_Request)

static inline AirRef_Request_ref_t AirRef_Request_create(flatbuffers_builder_t *B __AirRef_Request_formal_args)
{
    if (AirRef_Request_start(B)
        || AirRef_Request_requestType_add(B, v0)) {
        return 0;
    }
    return AirRef_Request_end(B);
}

static AirRef_Request_ref_t AirRef_Request_clone(flatbuffers_builder_t *B, AirRef_Request_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (AirRef_Request_start(B)
        || AirRef_Request_requestType_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, AirRef_Request_end(B));
}

__flatbuffers_build_union_field(1, flatbuffers_, AirRef_Message_content, AirRef_Content, AirRef_Message)
__flatbuffers_build_union_table_value_field(flatbuffers_, AirRef_Message_content, AirRef_Content, AirRefConf, AirRef_AirRefConf)
__flatbuffers_build_union_table_value_field(flatbuffers_, AirRef_Message_content, AirRef_Content, AirRefState, AirRef_AirRefState)
__flatbuffers_build_union_table_value_field(flatbuffers_, AirRef_Message_content, AirRef_Content, MachineState, AirRef_MachineState)
__flatbuffers_build_union_table_value_field(flatbuffers_, AirRef_Message_content, AirRef_Content, Request, AirRef_Request)

static inline AirRef_Message_ref_t AirRef_Message_create(flatbuffers_builder_t *B __AirRef_Message_formal_args)
{
    if (AirRef_Message_start(B)
        || AirRef_Message_content_add_value(B, v1)
        || AirRef_Message_content_add_type(B, v1.type)) {
        return 0;
    }
    return AirRef_Message_end(B);
}

static AirRef_Message_ref_t AirRef_Message_clone(flatbuffers_builder_t *B, AirRef_Message_table_t t)
{
    __flatbuffers_memoize_begin(B, t);
    if (AirRef_Message_start(B)
        || AirRef_Message_content_pick(B, t)) {
        return 0;
    }
    __flatbuffers_memoize_end(B, t, AirRef_Message_end(B));
}

#include "flatcc/flatcc_epilogue.h"
#endif /* AIRREF_BUILDER_H */
