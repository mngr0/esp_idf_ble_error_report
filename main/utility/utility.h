#ifndef UTILITY_H_
#define UTILITY_H_
#include <stdio.h>

#include "i2c_common/i2c_common.h"
#include "packet_manager/packet_manager.h"

#define MAX_FILE_SIZE  104876//Bytes //1 MiB

#define SEPARATOR_DEF {0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,0xa4,} 

//filename:  DATA_LOG.TXT -> l' ordine alfabetico e' anche l' ordine temporale, se si scrivono i mesi come 03
//example 202010161813_LOG.TXT 2020/10/16 18:13

//each line of log contains a packet. which is a complete set of frames, plus the date



//init utility -> search for last used file


//show files over bt/wifi

//query logs over a given period of time
//lo faccio solo se serve davvero
// get list of files

//query last fault
//lo faccio solo se serve davvero

//PROVARE A USARE BLUFI, INTEGRANDOLO IN QUESTO PROGETTO, IN ALTERNATIVA REIMPLEMENTARE IN MODO BECERO
//BT riceve in chiaro credenziali Wifi, e le salva in flash
//a boot cerca di collegarsi
//riporta lo stato della connessione wifi via bt


int log_received_message(rtc_time_t *time, packet_received_t* packet);
//add received message to log file (packet_received pkt)
// - get last edited file (if file_pointer_to_last_used==NULL)
// - check if size can be increased
// - - create new file if necessary (check remaining disk space on SD card)
// - append to file


void do_test_spi_sd();

#endif