#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL
typedef unsigned char uc;

const uc digit[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f }; //���� 0~9
const uc fnd_sel[4] = { 0x08,0x04,0x02,0x01 }; //sel0~4
const uc dot = 0x80;
int main() {
	int i, j, k, n;

	// ��µ� ���ڸ� �����ϴ� ����
	uc num[4] = { digit[0], digit[0] + dot, digit[0], digit[0] };

	DDRC = 0xFF;      // C ��Ʈ�� FND ��� ��Ʈ�� ��� (���� 0~9)
	DDRG = 0x0F;      // G ��Ʈ�� � FND�� ������ ������ �����ϴ� ��� ��Ʈ�� ��� (sel0~1)

	for (i = 0;; i++) {
		if (i == 10)
			i = 0;             // 99.99�ʿ��� �ʱ�ȭ
		num[0] = digit[i];
		for (j = 0; j < 10; j++) {
			num[1] = digit[j] + dot;    // 2��° ���ڴ� ��(dot) ����
			for (k = 0; k < 10; k++) {
				num[2] = digit[k];
				for (n = 0; n < 10; n++) {
					num[3] = digit[n];
					for (i = 0; i < 4; i++) {
						PORTC = _num[i];
						PORTG = fnd_sel[i];
						_delay_us(2500);        // ������� 1/100(s)
					}  
				}
			}
		}
	}
}
