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
	TCNT0 = melody1[mel_idx];
}

//interrupt 4(switch 4) buzzer를 끌수 있다.
ISR(INT4_vect) {
	TIMSK ^= 0X01;  //enable/Disable overflow interrupt
	_delay_ms(10);


}

//interrupt 5(switch 5) 한자리 높은 음계를 표현
ISR(INT5_vect) {
	mel_idx = (mel_idx + 1) % 8;
	_delay_ms(10);
}

int main() {
	DDRB = 0x10; //B포트를 버저 출력 포트로 사용
	DDRE = 0xCF; //E포트를 스위치 입력 포트로 사용
	EICRB = 0x0A; //External interrup의 trigger 설정에 사용하는 register, 인터럽트 4,5's trigger를 falling edge
	EIMSK = 0x30; //External interrupt 의 개별적인 제어를 담당하는 register. 인터럽트 4,5 enable

	TCCR0 = 0x03;	//프리스케일러를 32분주로
	TIMSK = 0x00; //overflow 인터럽트를 Disable

	TCNT0 = melody1[mel_idx]; //Timer count(on this, means melody)

	sei();
	while (1);

	//}
}
