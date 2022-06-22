
#ifndef ADC_H_
#define ADC_H_

#include "chip.h"

#define ADC_CH0_PORT  0
#define ADC_CH0_PIN  23

#define adc_start()			Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING)
#define adc_read(temp)		readAdcVal(LPC_ADC0, ADC_CH1, &temp)

void adc_init(void);

#endif /* ADC_H_ */
