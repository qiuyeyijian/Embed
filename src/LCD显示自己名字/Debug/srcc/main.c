//====================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�2017.08, 2020.06
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//====================================================================

#define GLOBLE_VAR
#include "includes.h"      //������ͷ�ļ�

//----------------------------------------------------------------------
//����ʹ�õ����ڲ�����
//main.cʹ�õ��ڲ�����������

//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1.2�������䡿�����ж�
    DISABLE_INTERRUPTS;
    wdog_stop();

    // ��ʼ�����ʹ��ںͽ��մ���
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);
    LCD_Init();        //��ʼ��LCD
    LCD_ShowString(6, 300, BLUE, GRAY, "Hello, world");
    LCD_ShowString(72, 32, BLUE, GRAY, "������");
    LCD_DrawPoint_big(30, 30, RED);

    LCD_DrawLine(50, 50, 50, 150, RED);

    LCD_DrawRectangle(60, 60, 120, 120, YELLOW);

    LCD_DrawSurface(70, 70, 140, 140, BLUE);

    LCD_ShowString(120, 240, BLUE, GRAY, "��Ҷ����");

    // ʹ�ܴ��ڽ����ж�
    suart_enable_receive_interupt(SUART_RX);

    //��1.7�������䡿�����ж�
    ENABLE_INTERRUPTS;
    //��2��======��ѭ�����֣���β��========================================

    for (;;) {

    }
}



