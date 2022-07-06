set(SOURCES main.c)
idf_component_register(SRC_DIRS . peripherals air_ref ble ble/ble_lib utility state_machines
                    INCLUDE_DIRS . ../components/cs011_termostatica_mplab/firmware/src/termostatica ../components/cs014_mplab/firmware/src/chiller 

                    REQUIRES nvs_flash mbedtls bt fatfs gel i2c json )

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
