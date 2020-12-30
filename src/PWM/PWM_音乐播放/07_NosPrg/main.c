//====================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�2017.08, 2020.06
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//====================================================================

#define GLOBLE_VAR
#include "includes.h"  //������ͷ�ļ�

//----------------------------------------------------------------------
//����ʹ�õ����ڲ�����
// main.cʹ�õ��ڲ�����������
void delay_ms(uint32_t ms);
//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void) {
  //��1.2�������䡿�����ж�
  DISABLE_INTERRUPTS;
  wdog_stop();

  gSelect = 3;

  // ��ʼ�����ʹ��ںͽ��մ���
  //============================================================
  pwm_init(PWM_USER, 3, 5753, 0, PWM_EDGE, PWM_PLUS);
  timer_init(TIMER_USER, 1);
  uart_init(UART_User, 115200);

  // ʹ�ܴ��ڽ����ж�
  timer_enable_int(TIMER_USER);
  uart_enable_re_int(UART_User);

  //��1.7�������䡿�����ж�
  ENABLE_INTERRUPTS;
  //��2��======��ѭ�����֣���β��========================================
  for (;;) {
    playStart();
    switch (gSelect) {
      case 0:
        playHappyBirthday();
        break;
      case 1:
        playFarewell();
        break;
      case 2:
        playMyth();
        break;
      case 3:
        playTenderLines();
        break;
      default:
        playStart();
        break;
    }

    // SPI���ݷ���
    printf("Hello, World!\n");
  }
}

// ��ʱ����
void delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < (6000 * ms); i++) {
    __asm("nop");
  }
}
