#include "includes.h"
#include "BswDrv_Adc.h"
#include "BswDrv_Delay.h"

static void BswDrv_ADC_TempSensorInit(void);

void BswDrv_ADC_TempSensorInit(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_APB2_CKAHB_DIV4);

    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);  
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);


    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_55POINT5);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* 16 times sample, 4 bits shift */
    // adc_oversample_mode_config(ADC0, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, ADC_OVERSAMPLING_RATIO_MUL16);
    // adc_oversample_mode_enable(ADC0);

    adc_tempsensor_vrefint_enable();
    /* enable ADC interface */
    
    adc_enable(ADC0);
    BswDrv_SoftDelay_ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

}

 
void BswDrv_ADC_Start(void)
{
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

float BswDrv_ADC_GetSensorTemp(void)
{
    float temperature;
    
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    while(SET != adc_flag_get(ADC0, ADC_FLAG_EOC)){
    }
    uint16_t adc_value = ADC_RDATA(ADC0);
	
    temperature = (1.43 - adc_value*3.3/4096) * 1000 / 4.3 + 25;
    return  temperature;
}


void BswDrv_ADC_Init(void)
{
    BswDrv_ADC_TempSensorInit();
}




