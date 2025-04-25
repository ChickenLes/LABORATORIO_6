
//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// IE2023: PROGRAMACION DE MICROCONTROLADORES
// PRELAB6.c
// AUTOR: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: PRELABORATORIO 4
// HARDWARE: ATMEGA328P
// CREADO: 4/24/2025 5:58:32 PM
// ULTIMA MODIFICACION: 4/24/2025 
// DESCRIPCIÓN:
//-----------------------------------------------
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>  



//VARIABLES GLOBALES
volatile uint8_t DATO_RECIBIDO = 0;	//VARIABLE CON EL DATO A RECIBIR
volatile uint8_t DATO_ENVIADO = 0;	//VARIABLE CON EL DATO ENVIADO
volatile uint16_t VALOR_ADC = 0;	//VALOR ACTUAL DEL ADC
volatile uint32_t COUNTER = 0;		//CONTADOR1




/////////////////////////////////////////CONFIGURANDO VECTORES////////////////////////////////////
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



////////////////////////////////////////////INTERRUPCION//////////////////////////////////////////
ISR(USART_RX_vect) {  
	DATO_RECIBIDO = UDR0;
	DATO_ENVIADO = 1;
}

void ENVIAR_UART(char c) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}


uint16_t LEER_ADC() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

///////////////////////////////////////////CONFIGURANDO SALIDAS///////////////////////////
void LED_conf() {
	DDRD |= 0b11111100;  //PD2-PD7 
	DDRB |= 0b00000011;  //PB0-PB1 
}

////////////////////////////////////////CONVIRTIENDO LEDS/////////////////////////////////
void MOSTRAR_LED(uint8_t data) {
	PORTD = (PORTD & 0b00000011) | ((data << 2) & 0b11111100); 
	PORTB = (PORTB & 0b11111100) | ((data >> 6) & 0b00000011);  
}

//////////////////////////////MAIN/////////////////////////////////////////////////
int main() {
	UART_conf();
	ADC_conf();
	LED_conf();

	uint8_t GUARDAR_VALOR = 0;
	uint32_t CONTADOR2 = 0;	//ULTIMO TIEMPO ENVIADO

	while (1) {
		_delay_ms(1);
		COUNTER++;

		//LEER VALOR DEL ADC
		VALOR_ADC = LEER_ADC();

		//PRIORIDAD PARA UART, SI RECIBE VALOR MOSTRAR EN LED
		if (DATO_ENVIADO) {
			GUARDAR_VALOR = DATO_RECIBIDO;
			ENVIAR_UART(DATO_RECIBIDO);
			ENVIAR_UART('\n');
			DATO_ENVIADO = 0;
		}

		//NO FUNCIONA CON ESTO, SI MOSTRAMOS TAMBIEN EL VALOR DEL ADC EN LOS LEDS
		// GUARDAR_VALOR = VALOR_ADC >> 2;

		MOSTRAR_LED(GUARDAR_VALOR);

		//MOSTRAR EN DISPLAY LUEGO DE 500MS LUEGO DE ENVIAR EN ADC
		if (COUNTER - CONTADOR2 >= 500) {
			char buffer[10];
			sprintf(buffer, "ADC: %u\n", VALOR_ADC); // "u" VALOR DECIMAL, "X" hexa y .2fV voltaje
			for (char *p = buffer; *p != '\0'; p++) ENVIAR_UART(*p);
			CONTADOR2 = COUNTER;
		}

	}
}