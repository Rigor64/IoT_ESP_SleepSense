#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_adc/adc_cali.h"


void app_main(void)
{
    //esp_err_t ret;
    int adc_read;
    int mv_output;

    
    adc_oneshot_unit_handle_t handle = NULL;

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_new_unit(&init_config1, &handle);
   
    adc_oneshot_chan_cfg_t config = {

        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_6,
    };

    adc_oneshot_config_channel(handle, ADC_CHANNEL_3, &config);

    adc_cali_handle_t cali_handle = NULL; 


    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_6,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    
    adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);
    

    while(1)
    {
        adc_oneshot_read(handle, ADC_CHANNEL_3, &adc_read);
        printf("Adc channel-3 raw read result %d \n", adc_read);
        printf("\n\n");
        adc_cali_raw_to_voltage(cali_handle, adc_read, &mv_output);
        printf("ADC milivold output %d \n", mv_output);        

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    

}
