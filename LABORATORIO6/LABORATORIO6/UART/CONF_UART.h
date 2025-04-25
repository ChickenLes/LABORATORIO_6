/*
 * CONF_UART.h
 *
 * Created: 4/25/2025 5:23:41 PM
 *  Author: itzle
 */ 


#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

//VARIABLES GLOBALES
extern volatile uint8_t DATO_RECIBIDO;
extern volatile uint8_t DATO_ENVIADO;
extern volatile uint16_t VALOR_ADC;
extern volatile uint32_t COUNTER;

//FUNCIONES
void UART_conf(void);
void ADC_conf(void);
void LED_conf(void);
void ENVIAR_UART(char c);
uint16_t LEER_ADC(void);
void CADENA(char txt[]);
void MOSTRAR_LED(uint8_t data);



#endif /* UART_H_ */