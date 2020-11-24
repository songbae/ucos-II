Skip to content
Search or jump to��

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
2019 - EmbeddedSystem / 12161558_7��_���� / emb.c
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

//interrupt 4(����ġ1)
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
	DDRC = 0xFF;  //C��Ʈ�� FND ��� ��Ʈ�� ���(���� 0-9)
	DDRG = 0x0F;	//G��Ʈ�� � FND�� ������ ������ �����ϴ� ��� ��Ʈ�� ���(sel0-3)
	DDRE = 0xCF;	//E��Ʈ�� ����ġ �Է� ��Ʈ�� ���
	EICRB = 0x0A;	//External interrupt�� trigger ������ ����ϴ� register.
				//���ͷ�Ʈ  4,5's trigger�� falling edge
	EIMSK = 0x30;	//External interrup�� �������� ��� ����ϴ� register. 
				//���ͷ�Ʈ 4,5 enable
	SREG |= 1 << 7;	//��ü ���ͷ�Ʈ ���1

	for (;;) {
		display_find(cnt);
		if (sig == start) { //1�̸� Ÿ�̸����
			cnt = (cnt + 1) % 10000; //00.00~99.99
		}
	}
}
