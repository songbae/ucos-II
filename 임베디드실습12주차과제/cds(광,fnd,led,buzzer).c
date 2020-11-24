#include <avr/io.h>
#include <util/delay.h>
#define CDS_VALUE 871
#define F_CPU 16000000UL
void init_adc();
unsigned short read_adc();
void show_adc(unsigned short value);

char fnd_value[3] = {};
unsigned char digit[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x07,0x7f,0x67 };
unsigned char fnd_sel[4] = { 0x08,0x04,0x02,0x01 };
int main() {
	unsigned short value;
	DDRA = 0xff;
	DDRB = 0x10;
	init_adc();
	while (1) {
		value = read_adc();
		show_adc(value);
	}
}

void init_adc() {
	ADMUX = 0x00;
	// 00000000
	// REFS(1:0) = "00" AREF(+5V) 기준전압 사용
	// ADLAR = '0' 디폴트 오른쪽 정렬
	// MUX(4:0) = "00000" ADC0 사용, 단극 입력
	ADCSRA = 0x87;
	// 10000111
	// ADEN = '1' ADC를 Enable
	// ADFR = '0' single conversion 모드
	// ADPS(2:0) = "111" 프리스케일러 128분주
}

unsigned short read_adc() {
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= 0x40; // ADC start conversion, ADSC = '1'
	while ((ADCSRA & (0x10)) != 0x10); // ADC 변환 완료 검사
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;

	return value;
}

void show_adc(unsigned short value) {

	if (value < CDS_VALUE) {
		unsigned char count = 1;
		int i = 0;
		DDRC = 0xff;
		DDRG = 0x0f;
		while (1) {
			int p;
			PORTA = count;
			_delay_ms(10000);
			if (count <= 64)count = count * 2;
			else {
				for (i = 0; i < 4; i++) {
					PORTC = digit[i + 1];
					PORTG = fnd_sel[i];
					_delay_ms(30000);
				}
				if (PORTC == digit[4]) {
					for (p = 0; p < 20; p++) {
						buzzer(12);
						buzzer(8);
						_delay_ms(200);
					}
				}
				break;
			}
		}
	}
	else PORTA = 0x00;
}
void buzzer(int cnt) {
	int j;
	for (j = 0; j < cnt; j++) {
		PORTB = 0x10;
		_delay_ms(100);
		PORTB = 0x00;
		_delay_ms(100);
	}
}
