#include <stdio.h>
#include "esp_err.h"
#include "esp_blufi_api.h"
#include "esp_log.h"
#include "esp_blufi.h"
#include "ble/blufi.h"
#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#ifdef CONFIG_BT_BLUEDROID_ENABLED


esp_err_t esp_blufi_gap_register_callback(void)
{
    return esp_blufi_profile_init();
}

esp_err_t esp_blufi_host_and_cb_init(esp_blufi_callbacks_t *example_callbacks)
{
    esp_err_t ret = ESP_OK;

    ret = esp_blufi_register_callbacks(example_callbacks);
    if(ret){
        BLUFI_ERROR("%s blufi register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_blufi_gap_register_callback();
    if(ret){
        BLUFI_ERROR("%s gap register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    return ESP_OK;

}

#endif