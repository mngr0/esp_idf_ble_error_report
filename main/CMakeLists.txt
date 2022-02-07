set(SOURCES main.c)
idf_component_register(SRC_DIRS . peripherals airref_flatbuffer air_ref air_ref/serial_logger ble ble/services utility
                    INCLUDE_DIRS .
                    REQUIRES nvs_flash mbedtls bt fatfs gel i2c flatcc json)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
