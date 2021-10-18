idf_component_register(SRCS "gatts_table_creat_demo.c" 
                            "air_ref/serial_logger/logger_air_ref.c"

                            "ble/ble.c" 
                            "ble/ble_lib.c"

                            "ble/blufi.c"
                            "ble/blufi_init.c"
                            "ble/services/gatt_ar_state.c" 
                            "ble/services/gatt_m_state.c" 
                            "ble/services/gatt_ar_conf.c"  
                            "ble/services/gatt_log_state.c" 

                            "air_ref/air_ref.c"
                            "fb/builder.c"
                            "fb/emitter.c"
                            "fb/refmap.c"

                            "rtcc/mcp7940.c"


                            "logger_frame/logger_frame.c" 
                            "logger_frame/logger_hw.c" 
                    INCLUDE_DIRS "."
                                 "fb")
