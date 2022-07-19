set(SOURCES main.c)
idf_component_register(SRC_DIRS .  ble ble/ble_lib  
                    INCLUDE_DIRS .
                    REQUIRES nvs_flash mbedtls bt fatfs)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
