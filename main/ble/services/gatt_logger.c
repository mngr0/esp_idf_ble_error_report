// presentare schema ad android.
// inviare in formato json

// characteristica da cui leggere lo schema da utilizzare

// caratteristica su cui aggiungere/modificare/eliminare schemi

// preprocessor->

#include "cJSON.h"
#include <stdint.h>

#define CONFIGURATION_FIELD_SIZE 32
#define NAME_SIZE 32
#define MAX_ENUM_VALUES 32

typedef enum {
  field_type_none = 0,
  field_type_int32,
  field_type_int32_dinamic_min,
  field_type_int32_dinamic_max,
  field_type_enum,
} field_type_t;

typedef struct {
  uint8_t val;
  char name[NAME_SIZE];
} enum_values;

typedef struct {
  uint8_t idx;
  char name[NAME_SIZE];
  enum_values values[MAX_ENUM_VALUES];
} field_enum;

typedef struct {
  uint8_t idx;
  char name[NAME_SIZE];
  int32_t min;
  int32_t max;
} field_int32;

typedef struct {
  uint8_t idx;
  char name[NAME_SIZE];
  char min[NAME_SIZE];
  int32_t max;
} field_int32_dinamic_min;

typedef struct {
  uint8_t idx;
  char name[NAME_SIZE];
  int32_t min;
  char max[NAME_SIZE];

} field_int32_dinamic_max;

typedef struct {
  union {
    field_enum enum_val;
    field_int32 int32_tval;
    field_int32_dinamic_min int32_tval_dmin;
    field_int32_dinamic_max int32_tval_dmax;
  };
  field_type_t type;
} configuration_field_t;

typedef struct {
  int32_t device_type;
  int32_t device_version;
  configuration_field_t machine_conf[CONFIGURATION_FIELD_SIZE];
  configuration_field_t machine_status[CONFIGURATION_FIELD_SIZE];
  configuration_field_t routine_conf[CONFIGURATION_FIELD_SIZE];
  configuration_field_t routine_status[CONFIGURATION_FIELD_SIZE];
} configuration;

void parseJSON() {}

char schema[] = {"\"device_type\":1\
\"device_version\":1\
\"machine_conf\":[\
{\"name\":\"setpoint\"\
\"idx\":1\
\"min\":0\
\"max\":0\
}\
{\"name\":\"control_mode\"\
\"idx\":2\
\"values\":[{\"mode1\",\"val\":1},{\"mode2\",\"val\":2}]\
}\
]\
"};

void parseLIST(cJSON *array, configuration_field_t *list) {
  cJSON *tmp;
  cJSON *tmp2;
  for (int i = 0; i < CONFIGURATION_FIELD_SIZE; i++) {
    switch (list[i].type) {
    case field_type_none: {
      // exit loop
      break;
    }
    case field_type_int32: {
      tmp = cJSON_CreateObject();
      cJSON_AddStringToObject(tmp, "name", list[i].int32_tval.name);
      cJSON_AddNumberToObject(tmp, "idx", list[i].int32_tval.idx);
      cJSON_AddNumberToObject(tmp, "min", list[i].int32_tval.min);
      cJSON_AddNumberToObject(tmp, "max", list[i].int32_tval.max);
      cJSON_AddItemToArray(array, tmp);

      break;
    }
    case field_type_enum: {
      tmp = cJSON_CreateObject();
      cJSON_AddStringToObject(tmp, "name", list[i].enum_val.name);
      cJSON_AddNumberToObject(tmp, "idx", list[i].enum_val.idx);
      cJSON *values = cJSON_AddArrayToObject(tmp, "values");
      for (int j = 0; j < MAX_ENUM_VALUES; j++) {
        tmp2 = cJSON_CreateObject();
        cJSON_AddStringToObject(tmp2, "name", list[i].enum_val.values[j].name);
        cJSON_AddNumberToObject(tmp2, "value", list[i].enum_val.values[j].val);
        cJSON_AddItemToArray(values, tmp2);
      }
      cJSON_AddItemToArray(array, tmp);

      break;
    }
    case field_type_int32_dinamic_min: {
      tmp = cJSON_CreateObject();
      cJSON_AddStringToObject(tmp, "name", list[i].int32_tval_dmin.name);
      cJSON_AddNumberToObject(tmp, "idx", list[i].int32_tval_dmin.idx);
      cJSON_AddStringToObject(tmp, "min", list[i].int32_tval_dmin.min);
      cJSON_AddNumberToObject(tmp, "max", list[i].int32_tval_dmin.max);
      cJSON_AddItemToArray(array, tmp);
    }
    case field_type_int32_dinamic_max: {
      tmp = cJSON_CreateObject();
      cJSON_AddStringToObject(tmp, "name", list[i].int32_tval_dmax.name);
      cJSON_AddNumberToObject(tmp, "idx", list[i].int32_tval_dmax.idx);
      cJSON_AddNumberToObject(tmp, "min", list[i].int32_tval_dmax.min);
      cJSON_AddStringToObject(tmp, "max", list[i].int32_tval_dmax.max);
      cJSON_AddItemToArray(array, tmp);
    }
    }
  }
}

void parseSTRUCT(configuration *conf) {
  // createJSON MAIN

  cJSON *root;
  root = cJSON_CreateObject();
  parseLIST(cJSON_AddArrayToObject(root, "machine_conf"), conf->machine_conf);
  parseLIST(cJSON_AddArrayToObject(root, "routine_conf"), conf->machine_conf);
  parseLIST(cJSON_AddArrayToObject(root, "machine_status"), conf->machine_conf);
  parseLIST(cJSON_AddArrayToObject(root, "routine_status"), conf->machine_conf);
}



// thread BLE
// started by serail routine, after start done
// waits for connection, at connecion notify configs
// periodically receives updates from serial, guarantees fairness between
// them when write...

// no, parte subito, deve reportare errori di comunicazione