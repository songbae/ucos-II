/* GPIO�� LED �Ѱ� ���� 1�� ��
 LED 8���� ����Ǿ� �ִ� ��Ʈ : Port A(PA)
 - ��Ʈ7 : LED7(LD1), ��Ʈ6 : LED6(LD2)
 - ...
 - ��Ʈ1 : LED1(LD7), ��Ʈ0 : LED0(LD8) */
 #include <avr/io.h>  //ATmage128 �������� ����
 int main(){
 	DDRA= 0xff; // ��Ʈ A�� ��� ��Ʈ�� ��� 
 	PORTA = 0xff; // 0xff = 0b11111111, LED ��� ON
 }
