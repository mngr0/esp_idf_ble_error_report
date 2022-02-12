// presentare schema ad android.
// inviare in formato json

// characteristica da cui leggere lo schema da utilizzare

// caratteristica su cui aggiungere/modificare/eliminare schemi

// preprocessor->

#include <stdint.h>

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

typedef enum {
  field_type_int32 = 0,
  field_type_enum,
} field_type_t;

typedef struct {
  uint8_t val;
  char name[32];
} enum_values;

typedef struct {
  uint8_t idx;
  char name[32];
  enum_values values[32];
} field_enum;

typedef struct {
  uint8_t idx;
  char name[32];
  int32_t min;
  int32_t max;
} field_int32;

typedef struct {
  union {
    field_enum enum_val;
    field_int32 int32_tval;
  };
  field_type_t type;
} configuration_field_t;

typedef struct {
  configuration_field_t machine_conf[32];
  configuration_field_t machine_status[32];
  configuration_field_t routine_conf[32];
  configuration_field_t routine_status[32];
} configuration;
