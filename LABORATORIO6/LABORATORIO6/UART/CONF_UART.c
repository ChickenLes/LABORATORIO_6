/*
 * CONF_UART.c
 *
 * Created: 4/24/2025 11:50:22 PM
 *  Author: itzle
 */ 
#include "CONF_UART.h"

//VARIABLES GLOBALES
volatile uint8_t DATO_RECIBIDO = 0;
volatile uint8_t DATO_ENVIADO = 0;
volatile uint16_t VALOR_ADC = 0;
volatile uint32_t COUNTER = 0;

//////////////////////CONFIGURANDO UART////////////////////
void UART_conf() {
	UBRR0H = 0;
	UBRR0L = 103;  //9600 baudios
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  //INTERRUPCION RX, HABILITAR TX Y RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  //8BITS, SIN PARIDAD, VALOR DE STOP 1BIT
	sei();
}

//////////////////////CONFIGURANDO ADC/////////////////////
void ADC_conf() {
	ADMUX = (1 << REFS0);  //VCC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  //PRESCALER 128
}

//////////////////////CONFIGURANDO SALIDAS/////////////////
void LED_conf() {
	DDRD |= 0b11111100;  //PD2-PD7
	DDRB |= 0b00000011;  //PB0-PB1
}

////////////////////////INTERRUPCION UART//////////////////
ISR(USART_RX_vect) {
	DATO_RECIBIDO = UDR0;    //GUARDAR UDR0
	DATO_ENVIADO = 1;        //ACTIVAR BANDERA DEBIDO A DATO RECIBIDO
}

////////////////////ENVIAR CARACTER POR UART///////////////
void ENVIAR_UART(char c) {
	while (!(UCSR0A & (1 << UDRE0)));    //ESPERAR HASTA QUE EL BUFFER ESTE VACIO
	UDR0 = c;    //ESCRIBE EL VALOR EN EL REGISTRO UDR0
}

//////////////////////LEEMOS EL ADC////////////////////////
uint16_t LEER_ADC() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

//////////////////////ENVIAR CADENA////////////////////////
void CADENA(char txt[]) {
	for (uint8_t i = 0; txt[i] != '\0'; i++) {
		ENVIAR_UART(txt[i]);
	}
}

//////////////////////MOSTRAR EN LEDS/////////////////////
void MOSTRAR_LED(uint8_t data) {
	PORTD = (PORTD & 0b00000011) | ((data << 2) & 0b11111100);
	PORTB = (PORTB & 0b11111100) | ((data >> 6) & 0b00000011);
}