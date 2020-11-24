#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL
typedef unsigned char uc;

const uc digit[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f }; //숫자 0~9
const uc fnd_sel[4] = { 0x08,0x04,0x02,0x01 }; //sel0~4
const uc dot = 0x80;
int main() {
	int i, j, k, n;

	// 출력될 숫자를 저장하는 변수
	uc num[4] = { digit[0], digit[0] + dot, digit[0], digit[0] };

	DDRC = 0xFF;      // C 포트를 FND 출력 포트로 사용 (숫자 0~9)
	DDRG = 0x0F;      // G 포트를 어떤 FND를 선택할 것인지 결정하는 출력 포트로 사용 (sel0~1)

	for (i = 0;; i++) {
		if (i == 10)
			i = 0;             // 99.99초에서 초기화
		num[0] = digit[i];
		for (j = 0; j < 10; j++) {
			num[1] = digit[j] + dot;    // 2번째 숫자는 점(dot) 포함
			for (k = 0; k < 10; k++) {
				num[2] = digit[k];
				for (n = 0; n < 10; n++) {
					num[3] = digit[n];
					for (i = 0; i < 4; i++) {
						PORTC = _num[i];
						PORTG = fnd_sel[i];
						_delay_us(2500);        // 숫자출력 1/100(s)
					}  
				}
			}
		}
	}
}
