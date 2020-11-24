#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define F_CPU 16000000UL
typedef unsigned char uc;

const uc digit[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f }; //���� 0~9
const uc fnd_sel[4] = { 0x08,0x04,0x02,0x01 }; //sel0~4
const uc led[8] = { 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80 }; //0~7 sel
const uc dot = 0x80;
void printNum(uc* num, int k) {
	PORTA = led[k];
	PORTC = num[0];
	PORTG = fnd_sel[0];
	_delay_us(2000000);

}

int main() {
	int i, j, k, n;

	// ��µ� ���ڸ� �����ϴ� ����
	uc num[4] = { digit[0], digit[0], digit[0], digit[0] };

	DDRC = 0xFF;      // C ��Ʈ�� FND ��� ��Ʈ�� ��� (���� 0~9)
	DDRG = 0x0F;      // G ��Ʈ�� � FND�� ������ ������ �����ϴ� ��� ��Ʈ�� ��� (sel0~1)
	DDRA = 0xFF;
	for (i = 0;; i++) {
		int a = random() % 8;
		num[0] = digit[a];
		printNum(num, a);
	}
}