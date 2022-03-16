
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

static bool machine_status_ready_to_send;
static bool routine_status_ready_to_send;

static bool machine_conf_ready_to_send;
static bool routine_conf_ready_to_send;


void set_machine_status_ready_to_send(){
    machine_status_ready_to_send=true;
}

void set_routine_status_ready_to_send(){
    routine_status_ready_to_send=true;
}

void check_send_conf(){
    if(machine_conf_ready_to_send){
        //send
    }
    if(routine_conf_ready_to_send){
        //send
    }
}

void task_notififier(){
    machine_status_ready_to_send=false;
    routine_status_ready_to_send=false;
    while(1){
        while(!machine_status_ready_to_send){
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        //send machine status
         while(!routine_status_ready_to_send){
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        //send routine status       

    }
    //

}