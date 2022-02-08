#ifndef VALIDATOR_FIELD_H_
#define VALIDATOR_FIELD_H_

#include <stdint.h>
#include <stdbool.h>


#define VALIDATOR_STATIC(name1, min_val, max_val )						\
bool name1 ## _validator(int32_t new_val){								\
	return ((new_val>=min_val ) && (new_val<max_val));					\
}


#define VALIDATOR_DYNAMIC_MIN(name1, conf, idx_min_val, max_val )		\
bool name1 ## _validator(int32_t new_val){								\
	return ((new_val>=conf[idx_min_val].value) && (new_val<max_val));	\
}

#define VALIDATOR_DYNAMIC_MAX(name1, conf, min_val, idx_max_val )		\
bool name1 ## _validator(int32_t new_val){								\
	return ( (new_val>=min_val )  && (new_val<conf[idx_max_val].value));	\
}

typedef bool (*fun_validator)(int32_t new_val);

typedef struct {
	int32_t value;
	fun_validator validate_value;
}validated_field;



#endif