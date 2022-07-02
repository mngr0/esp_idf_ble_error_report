set(SOURCES main.c)
idf_component_register(SRC_DIRS . peripherals air_ref ble ble/ble_lib utility
                    INCLUDE_DIRS .
                    REQUIRES nvs_flash mbedtls bt fatfs gel i2c json)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
