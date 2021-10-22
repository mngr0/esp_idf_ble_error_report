set(SOURCES main.c)
idf_component_register(SRC_DIRS . peripherals fb air_ref air_ref/serial_logger ble ble/services utility
                    INCLUDE_DIRS . fb
                    REQUIRES nvs_flash mbedtls bt fatfs gel i2c)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
