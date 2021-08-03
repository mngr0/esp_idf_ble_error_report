// /*
//    This example code is in the Public Domain (or CC0 licensed, at your option.)

//    Unless required by applicable law or agreed to in writing, this
//    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//    CONDITIONS OF ANY KIND, either express or implied.
// */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// /* Attributes State Machine */
// enum
// {
//     IDX_SVC,
//     IDX_CHAR_A,
//     IDX_CHAR_VAL_A,
//     // IDX_CHAR_CFG_A,

//     IDX_CHAR_B,
//     IDX_CHAR_VAL_B,

//     IDX_CHAR_C,
//     IDX_CHAR_VAL_C,

//     IDX_CHAR_D,
//     IDX_CHAR_VAL_D,
//     // IDX_M_STATE,
//     // IDX_AR_STATE,
//     // IDX_AR_CONF,
//     // IDX_AR_CONF_STATE,
//     // IDX_BLUEFI,
//     HRS_IDX_NB,
// };

// typedef enum
// {
//     GATT_AR_STATE_IDX_SERVICE,
//     GATT_AR_STATE_IDX_CHAR,
//     GATT_AR_STATE_IDX_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_ACTUAL_SPEED_INDEX_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_ACTUAL_SPEED_INDEX_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_LAST_SPEED_CHANGE_TIME_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_LAST_SPEED_CHANGE_TIME_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_SPEED_TO_COMMAND_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_SPEED_TO_COMMAND_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_LAST_STOP_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_LAST_STOP_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_IS_BLOCKED_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_IS_BLOCKED_VALUE,
//     // GATT_AR_STATE_IDX_COMPRESSOR_IS_RUNNING_CHAR,
//     // GATT_AR_STATE_IDX_COMPRESSOR_IS_RUNNING_VALUE,
//     // GATT_AR_STATE_IDX_FAN_SPEED_TO_COMMAND_CHAR,
//     // GATT_AR_STATE_IDX_FAN_SPEED_TO_COMMAND_VALUE,
//     // GATT_AR_STATE_IDX_FAN_TIME_LAST_COMMAND_CHAR,
//     // GATT_AR_STATE_IDX_FAN_TIME_LAST_COMMAND_VALUE,
//     GATT_AR_STATE_NB
// } gatt_ar_state_t;

// typedef enum
// {
//     GATT_M_STATE_IDX_SERVICE,
//     GATT_M_STATE_IDX_CHAR,
//     GATT_M_STATE_IDX_VALUE,
//     // GATT_M_STATE_IDX_EVAPORATION_PRESSURE_CHAR,
//     // GATT_M_STATE_IDX_EVAPORATION_PRESSURE_VALUE,
//     // GATT_M_STATE_IDX_CONDENSATION_PRESSURE_CHAR,
//     // GATT_M_STATE_IDX_CONDENSATION_PRESSURE_VALUE,
//     // GATT_M_STATE_IDX_TEMPERATURE_GAS_SCARICO_CHAR,
//     // GATT_M_STATE_IDX_TEMPERATURE_GAS_SCARICO_VALUE,
//     // GATT_M_STATE_IDX_TEMPERATURE_ENVIRONMENT_CHAR,
//     // GATT_M_STATE_IDX_TEMPERATURE_ENVIRONMENT_VALUE,
//     // GATT_M_STATE_IDX_TEMPERATURE_GAS_RITORNO_CHAR,
//     // GATT_M_STATE_IDX_TEMPERATURE_GAS_RITORNO_VALUE,
//     // GATT_M_STATE_IDX_TEMPERATURE_EXTRA_CHAR,
//     // GATT_M_STATE_IDX_TEMPERATURE_EXTRA_VALUE,
//     // GATT_M_STATE_IDX_IMC102_COMMUNICATION_CHAR,
//     // GATT_M_STATE_IDX_IMC102_COMMUNICATION_VALUE,
//     // GATT_M_STATE_IDX_IMC102_MOTOR_CHAR,
//     // GATT_M_STATE_IDX_IMC102_MOTOR_VALUE,
//     // GATT_M_STATE_IDX_IMC102_STATUS_SPEED_CHAR,
//     // GATT_M_STATE_IDX_IMC102_STATUS_SPEED_VALUE,
//     // GATT_M_STATE_IDX_IMC102_STATUS_TARGET_SPEED_CHAR,
//     // GATT_M_STATE_IDX_IMC102_STATUS_TARGET_SPEED_VALUE,
//     // GATT_M_STATE_IDX_IMC102_STATUS_ERROR_CHAR,
//     // GATT_M_STATE_IDX_IMC102_STATUS_ERROR_VALUE,
//     // GATT_M_STATE_IDX_IMM101_COMMUNICATION_CHAR,
//     // GATT_M_STATE_IDX_IMM101_COMMUNICATION_VALUE,
//     // GATT_M_STATE_IDX_IMM101_MOTOR_CHAR,
//     // GATT_M_STATE_IDX_IMM101_MOTOR_VALUE,
//     // GATT_M_STATE_IDX_IMM101_STATUS_SPEED_CHAR,
//     // GATT_M_STATE_IDX_IMM101_STATUS_SPEED_VALUE,
//     // GATT_M_STATE_IDX_IMM101_STATUS_TARGET_SPEED_CHAR,
//     // GATT_M_STATE_IDX_IMM101_STATUS_TARGET_SPEED_VALUE,
//     // GATT_M_STATE_IDX_IMM101_STATUS_ERROR_CHAR,
//     // GATT_M_STATE_IDX_IMM101_STATUS_ERROR_VALUE,
//     // GATT_M_STATE_IDX_PIN_ENABLE_CHAR,
//     // GATT_M_STATE_IDX_PIN_ENABLE_VALUE,
//     // GATT_M_STATE_IDX_AR_ERROR_CHAR,
//     // GATT_M_STATE_IDX_AR_ERROR_VALUE,
//     // GATT_M_STATE_IDX_AR_STATUS_CHAR,
//     // GATT_M_STATE_IDX_AR_STATUS_VALUE,
//     GATT_M_STATE_NB
// } gatt_m_state_t;

// // typedef enum
// // {
// //     GATT_AR_CONF_IDX_SERVICE,
// //     GATT_AR_CONF_IDX_FAN_TARGET_PRESSURE_CHAR,
// //     GATT_AR_CONF_IDX_FAN_TARGET_PRESSURE_VALUE,
// //     GATT_AR_CONF_IDX_FAN_COEFF_P_CHAR,
// //     GATT_AR_CONF_IDX_FAN_COEFF_P_VALUE,
// //     GATT_AR_CONF_IDX_FAN_COEFF_OFFSET_CHAR,
// //     GATT_AR_CONF_IDX_FAN_COEFF_OFFSET_VALUE,
// //     GATT_AR_CONF_IDX_FAN_MIN_PRESSURE_CHAR,
// //     GATT_AR_CONF_IDX_FAN_MIN_PRESSURE_VALUE,
// //     GATT_AR_CONF_IDX_FAN_MAX_PRESSURE_CHAR,
// //     GATT_AR_CONF_IDX_FAN_MAX_PRESSURE_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_TARGET_PRESSURE_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_TARGET_PRESSURE_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_ACTIVATION_OFFSET_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_ACTIVATION_OFFSET_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_ACTION_DELAY_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_ACTION_DELAY_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_START_INTERVAL_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_START_INTERVAL_VALUE,
// //     ATT_AR_CONF_IDX_LP_LOW_PRESSURE_LIMIT_CHAR,
// //     ATT_AR_CONF_IDX_LP_LOW_PRESSURE_LIMIT_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED1_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED1_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED2_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED2_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED3_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED3_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED4_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED4_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED5_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED5_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED6_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED6_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED7_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED7_VALUE,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED8_CHAR,
// //     GATT_AR_CONF_IDX_COMPRESSOR_SPEED8_VALUE,
// //     GATT_AR_CONF_NB
// // } gatt_ar_conf_t;

// // typedef enum
// // {
// //     GATT_AR_CONF_IDX_SERVICE,
// //     GATT_AR_CONF_IDX_CHAR,
// //     GATT_AR_CONF_IDX_VALUE,
// //     GATT_AR_CONF_NB
// // } gatt_ar_conf_t;

// // typedef enum
// // {
// //     GATT_BLUEFI_IDX_SERVICE,
// //     GATT_BLUEFI_NB
// // } gatt_bluefi_t;

// // #define BLINK_GPIO1 2
// // #define BLINK_GPIO2 15
// // #define BLINK_GPIO3 13
// // #define BTN_GPIO 3