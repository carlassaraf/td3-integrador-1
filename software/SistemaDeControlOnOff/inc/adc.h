
#ifndef ADC_H_
#define ADC_H_

#include "chip.h"
/* PORT donde esta el ADC0 */
#define ADC_CH0_PORT  0
/* Pin que usa el ADC0 */
#define ADC_CH0_PIN  23
/* Macro para inicializar una conversion */
#define adc_start()			Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING)
/* Macro para obtener el valor del ADC */
#define adc_read(x)			Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &x)
/* Prototipos */
void adc_init(void);

#endif /* ADC_H_ */
