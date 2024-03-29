# docs

## Herencia de prioridad

### Sin MUTEX

![](nomutex.png)

### Con MUTEX

![](mutex.png)

La diferencia entre el caso 1 y caso 2 se encuentra en que en el primero se ejecutará la tarea con mayor prioridad indefinidamente mientras que en el caso 2 utilizando MUTEX la tarea con un nivel inferior de prioridad puede trabajar “como si” tuviese un nivel de prioridad mayor. Con esto se alternan las tareas de prioridad mayor y la de prioridad menor a la hora de utilizar el recurso del microcontrolador.

## Pin Map

| Pn.xx | Funcion | Modulo |
| --- | --- | --- |
| P0.23 | ADC0.0 | Salida del etapa LM35 |
| P2.0 | P2.0 | Display 7 segmentos (A) |
| P2.1 | P2.1 | Display 7 segmentos (B) |
| P2.2 | P2.2 | Display 7 segmentos (C) |
| P2.3 | P2.3 | Display 7 segmentos (D) |
| P2.4 | P2.4 | Display 7 segmentos (E) |
| P2.5 | P2.5 | Display 7 segmentos (F) |
| P2.6 | P2.6 | Display 7 segmentos (G) |
| P2.10 | P2.10 | Transistor digito 1 display 7 segmentos |
| P2.11 | P2.11 | Transistor digito 2 display 7 segmentos |
| P2.12 | P2.12 | Transistor digito 3 display 7 segmentos |
| P0.15 | SSP0 SCK | SSP0 clock para la SD |
| P0.16 | SSP0 SSEL0 | SSP0 slave select para la SD |
| P0.17 | SSP0 MISO | SSP0 MISO para la SD |
| P0.18 | SSP0 MOSI | SSP0 MOSI para la SD |
| P0.7 | P0.7 | Salida para activar cooler |
| P0.8 | P0.8 | Salida para calefaccion |
| P0.9 | P0.9 | Salida para refrigeracion |
| P0.21 | P0.21 | Tecla up de menu |
| P0.22 | P0.22 | Tecla down de menu |
| P0.27 | P0.27 | Tecla enter de menu |
| P0.28 | P0.3 | Tecla para cambio de display |
| P2.10 | P2.10 | LED RGB para tecla up |
| P2.11 | P2.11 | LED RGB para tecla down |
| P2.12 | P2.12 | LED RGB para tecla enter |