Skip to content
Search or jump to…

Pull requests
Issues
Marketplace
Explore

@tjdqo94
kimhyeyun
/
2019 - EmbeddedSystem
1
00
Code
Issues
Pull requests
Actions
Projects
Wiki
Security
Insights
2019 - EmbeddedSystem / 12161558_7차_과제 / emb.c
@kimhyeyun
kimhyeyun homework
Latest commit f87eec5 on 20 Mar
History
1 contributor
69 lines(56 sloc)  1.35 KB

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define F_CPU 16000000UL
#define start 1
#define stop 0

typedef unsigned char uc;

const uc digit[10] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x6f };
const uc fnd_sel[4] = { 0x01,0x02,0x04,0x08 };
const uc dot = 0x80;

volatile int cnt = 0;
volatile int sig = 0;

//interrupt 4(스위치1)
ISR(INT4_vect) {
	if (sig == start) sig = stop;
	else sig = start;
	_delay_ms(10);
}

ISR(INT5_vect) {
	if (sig == stop)cnt = 0;
	_delay_ms(10);
}


void display_find(int c) {
	int i;
	uc fnd[4];
	fnd[0] = digit[c% 10];
	fnd[1] = digit[(c / 10) % 10];
	fnd[2] = digit[(c / 100) % 10] + dot;
	fnd[3] = digit[(c / 1000) % 10];

	for (i = 0; i < 4; i++) {
		PORTC = fnd[i];
		PORTG = fnd_sel[i];
		_delay_us(25000); //delay 2.5ms
	}
}

int main() {
	DDRC = 0xFF;  //C포트를 FND 출력 포트로 사용(숫자 0-9)
	DDRG = 0x0F;	//G포트를 어떤 FND를 선택할 것인지 결정하는 출력 포트로 사용(sel0-3)
	DDRE = 0xCF;	//E포트를 스위치 입력 포트로 사용
	EICRB = 0x0A;	//External interrupt의 trigger 설정에 사용하는 register.
				//인터럽트  4,5's trigger를 falling edge
	EIMSK = 0x30;	//External interrup의 개별적인 제어를 담당하는 register. 
				//인터럽트 4,5 enable
	SREG |= 1 << 7;	//전체 인터럽트 허용1

	for (;;) {
		display_find(cnt);
		if (sig == start) { //1이면 타이머출력
			cnt = (cnt + 1) % 10000; //00.00~99.99
		}
	}
}
