#ifndef ERROR_H_
#define ERROR_H_


typedef enum {
	error_state_none=0,
	error_state_warning,
	error_state_critical
} error_state_t;

typedef struct{
	error_state_t error_state;
	int16_t error_code;  
} error_report_t;


#endif /* ERROR_H_ */