#include "includes.h"
#define F_CPU	16000000UL	// CPU frequency = 16 Mhz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define CDS_VALUE 871
#define  TASK_STK_SIZE  OS_TASK_DEF_STK_SIZE
#define  N_TASKS    5// task 개수 5개 
//사용할 문자 win , lose , T 
#define W1 19
#define W2 20
#define I 1
#define N 21
#define L 22
#define O 0
#define	S 5 
#define E 23
#define T1 25
#define T2 24
#define PLAY_TIME 150

#define ATS75_ADDR 0x98 // 0b10011000, 7비트를 1비트 left shift
#define ATS75_TEMP_REG 0 //ats75 레지스터의 bit
#define ATS75_CONFIG_REG 1 //ats75 레지스터의 bit

// 음계 처리 
#define DO 17
#define RE 43
#define MI 66
#define FA 77
#define SOL 97
#define LA 114
#define TI 129
#define UDO 137
#define ON 1
#define OFF 0

OS_STK       TaskStk[N_TASKS][TASK_STK_SIZE];
OS_FLAG_GRP* led_grp;
OS_FLAG_GRP* buzzer_grp;
OS_FLAG_GRP* buzzer1_grp;
OS_EVENT* Mbox[3];// test 단계 값 전송  0일때 test case 전송 1: fnd 출력
OS_EVENT* queue_to_led;// 스위치 누른 횟수를 led로 전송 
OS_EVENT* Sem;// 전역변수 보호 
void* led_queue[4];// 메시지 큐 
volatile INT8U	FndNum;
// 사용할 함수들 (task )
void  Test(void* data);
void  Fnd(void* data);
void  Led(void* data);
void Buzzer(void* data);
void Buzzer1(void* data);
unsigned short light();
void Test_case(unsigned short value);
void clear_fnd();//fnd 초기화 해주기 
volatile int test = 1,switch_state = OFF, switch_click = OFF, switch_cnt = -1, test_case = OFF, buzzer_state = OFF, melody_idx = 0, melody1_idx = 0,idx=0;
const unsigned char win_melody[2][25] = { 
	{SOL,MI,MI,SOL,MI,DO,RE,MI,RE,DO,MI,SOL,DO,SOL,DO,SOL,DO,SOL,MI,SOL,RE,FA,MI,RE,DO },
	{MI,MI,MI,MI,MI,FA,FA,MI,RE,RE,RE,FA,LA,SOL,MI,MI,MI,MI,MI,FA,LA,SOL,FA,MI,RE}
};//산토끼 음계, 아빠 힘내세요

INT8U err;
//타이머 기능
unsigned char FND_DATA[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f,0x77,0x7c,0x39, 0x5e,0x79,0x71,0x80,0x40,0x08,0x3c,0x1e,0x54,0x38,0x79,0x07,0x31
};
ISR(TIMER2_OVF_vect) {
	if (buzzer_state == ON) {
		PORTB = 0x00;
		buzzer_state = OFF;
	}
	else {
		PORTB = 0x10;
		buzzer_state = ON;
	}
	 TCNT2 = win_melody[idx][melody_idx];
}
//ISR(TIMER0_COMP_vect) {
//	if (buzzer_state == ON) {
//		PORTB = 0x00;
//		buzzer_state = OFF;
//	}
//	else {
//		PORTB = 0x10;
//		buzzer_state = ON;
//	}
//	TCNT0 = lose_melody[melody1_idx];
//}
ISR(INT4_vect) {
	OSSemPend(Sem, 0, &err);
	if (switch_state == OFF)PORTA = 0x00;
	switch_state = ON;
	switch_click = ON;
	if (switch_cnt < 8) {
		switch_cnt++;
		OSQPost(queue_to_led, &switch_cnt);
	}
	OSSemPost(Sem);

}
//스위치2 test case 결정 
ISR(INT5_vect) {
	OSSemPend(Sem, 0, &err);
	test_case = ON;
	OSSemPost(Sem);
}

int main(void) {

	int i;
	OSInit();
	OS_ENTER_CRITICAL();
	// 레지스터 값 설정 

	DDRA = 0xff;DDRB = 0x10;DDRE = 0xCF;
	EICRB = 0x0A;EIMSK = 0x30;
	SREG |= 1 << 7;// 전체 인터럽트 허용
	ADMUX = 0x00;ADCSRA = 0x87;
	TCCR2 = 0x03;
	TIMSK = 0x00;
	TCNT2 = win_melody[idx][melody_idx];
	Sem = OSSemCreate(1);
	for (i = 0; i < 3; i++)Mbox[i] = OSMboxCreate((void*)0);
	queue_to_led = OSQCreate(led_queue, 4);
	led_grp = OSFlagCreate(0x00, &err);
	buzzer_grp = OSFlagCreate(0x00, &err);
	buzzer1_grp = OSFlagCreate(0x00, &err);
	//task 생성
	OSTaskCreate(Fnd, (void*)0, (void*)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(Buzzer, (void*)0, (void*)&TaskStk[1][TASK_STK_SIZE - 1], 2);
	OSTaskCreate(Buzzer1, (void*)0, (void*)&TaskStk[2][TASK_STK_SIZE - 1], 3);
	OSTaskCreate(Test, (void*)0, (void*)&TaskStk[3][TASK_STK_SIZE - 1], 4);
	OSTaskCreate(Led, (void*)0, (void*)&TaskStk[4][TASK_STK_SIZE - 1], 5);
	OSStart();
	return 0;


}
//led task-> 게임-> 주어진 시간에 led를 모두 켜면 성공, 아니면 실패 
void Led(void* data) {
	int cnt;// 스위치 누른 횟수 
	char res;//결과값
	int end_time;// 종료 시간
	int cur_test; //현재 test 
	int time_cnt = 0;// 현재 시간 
	data = data;
	PORTA = 0x00;
	cur_test = *(int*)(OSMboxPend(Mbox[0], 0, &err));
	while (1) {
		end_time = PLAY_TIME - ((cur_test - 1) * 50);
		time_cnt++;
		//시간안에 끝내지못한경우 
		if (time_cnt >= end_time) {
			OSSemPend(Sem, 0, &err);
			PORTA = 0x00;
			res = 'F';
			OSMboxPost(Mbox[1], (void*)&res);//결과값 전송 
			time_cnt = 0;
			OSSemPost(Sem);
		}
		//  성공시 
		else if (switch_click == ON && res != 'F') {
			cnt = *(int*)(OSQPend(queue_to_led, 0, &err));
			switch_click = OFF;
			//성공시
			if (cnt == 7) {
				OSSemPend(Sem, 0, &err);
				PORTA = 0xff;
				res = 'P';
				OSMboxPost(Mbox[1], (void*)&res);
				time_cnt = 0;
				OSSemPost(Sem);
			}
			else {
				OSSemPend(Sem, 0, &err);
				PORTA += 0x80 >> cnt;
				OSSemPost(Sem);
			}
		}
		_delay_ms(50);
	}


}

//fnd 출력하기 게임결과 win or lose 
void Fnd(void* data) {
	char value;// 결과값 
	int cur_test;// test case 가 몇번째인디
	int pend_time = 300; //fnd 출력시간
	data = data;
	DDRC = 0xff;
	DDRG = 0x0f;
	while (1) {
		value = *(char*)(OSMboxPend(Mbox[1], 0, &err));//led결과
		//성공시 
		if (value == 'P') {
			while (pend_time--) {// pend시간이 0초가 될때까지 지속 
				PORTC = FND_DATA[N];
				PORTG = 0x01; // 1번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[I];
				PORTG = 0x02; // 2번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[W2];
				PORTG = 0x04; // 3번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[W1];
				PORTG = 0x08; // 4번째 자리
				_delay_ms(2);
			}
			OSFlagPost(buzzer_grp, 0xff, OS_FLAG_SET, &err);
		}
		else {// 실패한경우 
			while (pend_time--) {
				PORTC = FND_DATA[E];
				PORTG = 0x01; // 1번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[S];
				PORTG = 0x02; // 2번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[O];
				PORTG = 0x04; // 3번째 자리
				_delay_ms(2);
				PORTC = FND_DATA[L];
				PORTG = 0x08; // 4번째 자리
				_delay_ms(2);
			}
			OSFlagPost(buzzer1_grp, 0xff, OS_FLAG_SET, &err);
		}
		clear_fnd();
		pend_time = 300;
	}
}

//조도를 이용한 testcase 설정 하기 
void Test(void* data) {
	unsigned short value;
	data = data;
	while (1) {
		if (test_case == ON) {
			clear_fnd();
			OSMboxPost(Mbox[0], (void*)&test);
			OSFlagPend(led_grp, 0xff, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);

		}
		value = light();
		Test_case(value);
		//fnd test case 를 보여줌 
		PORTC = FND_DATA[test];
		PORTG = 0x01; // 1번째 자리
		_delay_ms(2);
		_delay_ms(2);
		PORTC = FND_DATA[25];
		PORTG = 0x04; // 3번째 자리
		_delay_ms(2);
		PORTC = FND_DATA[24];
		PORTG = 0x08; // 4번째 자리
		_delay_ms(2);
	}
}
//win 인경우 노래
//lose 인경우 다른 노래 출력 
void Buzzer1(void* data) {
	data = data;
	OSFlagPend(buzzer1_grp, 0xff, OS_FLAG_WAIT_SET_ALL, 0, &err);
	TIMSK = 0x40;
	idx = 1;
	TCNT2 = win_melody[1][melody_idx];
	// 동요 출력하기 
	while (melody_idx < 25) {
		melody_idx += 1;
		_delay_ms(500);

	}
	melody_idx = 0;
	OSFlagPost(led_grp, 0xff, OS_FLAG_SET, &err);
}
void Buzzer(void* data) {
	data = data;
	OSFlagPend(buzzer_grp, 0xff, OS_FLAG_WAIT_SET_ALL, 0, &err);
	idx = 0;
		TIMSK = 0x40;
		TCNT2 = win_melody[0][melody_idx];
		// 동요 출력하기 
		while (melody_idx < 25) {
			melody_idx += 1;
			_delay_ms(500);

		}
		melody_idx = 0;
		OSFlagPost(led_grp, 0xff, OS_FLAG_SET, &err);
	}
void clear_fnd() {
	PORTC = 0x00;
	PORTG = 0x01;
	_delay_ms(2);
	PORTG = 0x02;
	_delay_ms(2);
	PORTG = 0x04;
	_delay_ms(2);
	PORTG = 0x08;
	_delay_ms(2);
	PORTA = 0x00;
}


//조도를 변환하기 
unsigned short light() {
	unsigned char light_min, light_max;
	unsigned short value;
	ADCSRA |= 0x40;// 
	while ((ADCSRA & (0x10)) != 0x10);
	light_min = ADCL;
	light_max = ADCH;
	value = (light_max << 8) | light_min;// 변환된 조도값 설정 
	return value;
}

void Test_case(unsigned short value) {
	//어두워지는걸 측정 
	if (value < CDS_VALUE) {
		OSSemPend(Sem, 0, &err);
		test++;
		if (test == 4)test = 1;
		_delay_ms(100);
		OSSemPost(Sem);
	}
}
