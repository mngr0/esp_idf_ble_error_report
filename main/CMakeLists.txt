idf_component_register(SRCS "gatts_table_creat_demo.c" 
                            "air_ref/serial_logger.c" 
                            "ble/ble.c" 
                            "ble/services/gatt_ar_state.c" 
                            "ble/services/gatt_m_state.c" 
                            "ble/services/gatt_ar_conf.c"  
                            "ble/services/gatt_bluefi.c"
                    INCLUDE_DIRS "."
                                 "fb")

# add_subdirectory(fb)