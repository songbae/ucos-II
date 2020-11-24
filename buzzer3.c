#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/delay.h>

#define F_CPU 16000000UL

#define ON 1
#define OFF 0

#define DO 17
#define RE 43
#define MI 66
#define FA 77
#define SOL 97
#define LA 114
#define TI 129
#define UDO 137

typedef unsigned char uc;

const uc melody[25] = { MI,RE,DO,RE,MI,MI,MI,RE,RE,RE,MI,SOL,SOL,MI,RE,DO,RE,MI,MI,MI,RE,RE,MI,RE,DO };
const uc melody1[8] = { DO,RE,MI,FA,SOL,LA,TI,UDO };
volatile int mel_idx = 0;
volatile int state = OFF;

//Timer interrupt
ISR(TIMER0_OVF_vect) {
	if (state == ON) {
		PORTB = 0x00;
		state = OFF;
	}
	else {
		PORTB = 0x10;
		state = ON;
	}
	TCNT0 = melody[mel_idx];
}

//interrupt 4(switch 4) buzzer�� ���� �ִ�.
ISR(INT4_vect) {
	TIMSK ^= 0X01;  //enable/Disable overflow interrupt
	_delay_ms(10);


}

int main() {
	DDRB = 0x10; //B��Ʈ�� ���� ��� ��Ʈ�� ���
	DDRE = 0xCF; //E��Ʈ�� ����ġ �Է� ��Ʈ�� ���
	EICRB = 0x0A; //External interrup�� trigger ������ ����ϴ� register, ���ͷ�Ʈ 4,5's trigger�� falling edge
	EIMSK = 0x30; //External interrupt �� �������� ��� ����ϴ� register. ���ͷ�Ʈ 4,5 enable

	TCCR0 = 0x03;	//���������Ϸ��� 32���ַ�
	TIMSK = 0x00; //overflow ���ͷ�Ʈ�� Disable

	TCNT0 = melody[mel_idx]; //Timer count(on this, means melody)

	sei();
	while (1) {
		mel_idx = (mel_idx + 1) % 25;
		_delay_ms(10000);
	}

}
