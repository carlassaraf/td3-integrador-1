
#include <adc.h>


/* Inicialización del ADC */
void adc_init(void) {
	/* ADC config struct */
	static ADC_CLOCK_SETUP_T ADCSetup;
	/* Asocia el pin a la función de ADC */
	Chip_IOCON_PinMux(LPC_IOCON, ADC_CH0_PORT, ADC_CH0_PIN, IOCON_MODE_INACT, IOCON_FUNC1);
	/* Inicializa el ADC */
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	/* Conecta el ADC con el canal elegido */
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	/* Velocidad de conversión máxima */
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADC_MAX_SAMPLE_RATE);
	/* Habilito la interrupcion para ADC0 */
	NVIC_EnableIRQ(ADC0_IRQn);
	/* Habilita la interrupción de ADC */
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH0, ENABLE);
	/* Desactiva modo ráfaga */
	Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
}

