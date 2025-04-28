
//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// IE2023: PROGRAMACION DE MICROCONTROLADORES
// PRELAB6.c
// AUTOR: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: PRELABORATORIO 4
// HARDWARE: ATMEGA328P
// CREADO: 4/21/2025 2:27:32 PM
// ULTIMA MODIFICACION: 4/24/2025
// DESCRIPCIÓN:
//-----------------------------------------------
#define F_CPU 16000000UL

/////////////////////////LIBRERIAS/////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// VARIABLES GLOBALES
volatile uint8_t DATO_RECIBIDO = 0;    // VARIABLE CON EL DATO A RECIBIR
volatile uint8_t DATO_ENVIADO = 0;     // VARIABLE CON EL DATO ENVIADO
volatile uint16_t VALOR_ADC = 0;       // VALOR ACTUAL DEL ADC
volatile uint32_t COUNTER = 0;         // CONTADOR1
volatile uint8_t MODO = 0;             //SELECCION MODO 0:ESPERANDO, 1:LEER POTE, 2: INTRODUCIR UNA LETRA


/////////////////////////////////////////CONFIGURANDO VECTORES////////////////////////////////////
//////////////////////CONFIGURANDO UART////////////////////
void UART_conf() {
	UBRR0H = 0;
	UBRR0L = 103;  // 9600 baudios
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  // INTERRUPCION RX, HABILITAR TX Y RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8BITS, SIN PARIDAD, VALOR DE STOP 1BIT
	sei();
}

//////////////////////CONFIGURANDO ADC/////////////////////
void ADC_conf() {
	ADMUX = (1 << REFS0);  // VCC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // PRESCALER 128
}

////////////////////////////////////////////INTERRUPCION//////////////////////////////////////////
ISR(USART_RX_vect) {
	DATO_RECIBIDO = UDR0;   // GUARDAR UDR0
	DATO_ENVIADO = 1;       // ACTIVAR BANDERA DEBIDO A DATO RECIBIDO
}

///////////////////////////ENVIAR CARACTER POR UART///////////////
void ENVIAR_UART(char c) {
	while (!(UCSR0A & (1 << UDRE0)));  // ESPERAR HASTA QUE EL BUFF ESTE VACIO
	UDR0 = c;  // ESCRIBE EL VALOR EN EL REGISTRO UDR0
}

//////////////////////////LEEMOS EL ADC//////////////////////////
uint16_t LEER_ADC() {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

///////////////////////////////////////////CONFIGURANDO SALIDAS///////////////////////////
void LED_conf() {
	DDRD |= 0b11111100;  // PD2-PD7
	DDRB |= 0b00000011;  // PB0-PB1
}

//////////////////////ENVIAR CADENA////////////////////////
void CADENA(const char *txt) {
	for (uint8_t i = 0; txt[i] != '\0'; i++) {
		ENVIAR_UART(txt[i]);
	}
}

////////////////////////////////////////CONVIRTIENDO LEDS/////////////////////////////////
void MOSTRAR_LED(uint8_t data) {
	PORTD = (PORTD & 0b00000011) | ((data << 2) & 0b11111100);
	PORTB = (PORTB & 0b11111100) | ((data >> 6) & 0b00000011);
}

//////////////////////////////MOSTRAR MENU///////////////////////////////////////////////
void MOSTRAR_MENU() {
	CADENA("\nElige una opcion:\n");
	CADENA("1. Leer potenciometro\n");
	CADENA("2. Enviar algo a los leds\n");
}

//////////////////////////////MAIN/////////////////////////////////////////////////
int main() {
	UART_conf();
	ADC_conf();
	LED_conf();
	
	uint8_t GUARDAR_VALOR = 0;
	//uint32_t CONTADOR2 = 0;  //ULTIMO TIEMPO ENVIADO
	
	MOSTRAR_MENU();

	while (1) {
		_delay_ms(1);
		COUNTER++;
		
		//LEER EL VALOR DEL ADC
		VALOR_ADC = LEER_ADC();
		
		
		//
		if (DATO_ENVIADO)
		{
			//MODO SIN SELECCION
			if (MODO == 0) {
				
				//ACTIVANDO MODO 1 (POTE)
				if (DATO_RECIBIDO == '1')
				{
					MODO = 1;
					CADENA("\nValor del ADC(POTENCIOMETRO).\n");
					char BUFF[20];
					sprintf(BUFF, "Valor ADC: %u\n", VALOR_ADC);
					CADENA(BUFF);
					MOSTRAR_MENU();
					MODO = 0; //VOLVER A MOSTRAR SELECCIONES
				}
				//ACTIVANDO MODO 2(INGRESAR Y MOSTRAR)
				else if (DATO_RECIBIDO == '2')
				{
					MODO = 2;
					CADENA("\n Introducir una letra:\n");
				}
				
				else {
					CADENA("\n Seleccionar ´1´ o ´2´ ");
					MOSTRAR_MENU();
				}
			}
			//MODO LED
			else if (MODO == 2) {
				//ESPERAMOS A RECIBIR UNA LETRA
				GUARDAR_VALOR = DATO_RECIBIDO;
				ENVIAR_UART(DATO_RECIBIDO);
				ENVIAR_UART('\n');
				CADENA("DATO MOSTRADO EN LEDS. ");
				MOSTRAR_MENU();
				MODO = 0;
			}
			
			DATO_ENVIADO = 0; //REINICIAR BANDERA
		}
		
		MOSTRAR_LED(GUARDAR_VALOR);
	}
}