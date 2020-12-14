//======================================================================
//�ļ����ƣ�cpu.h��cpuͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200627
//======================================================================
#ifndef CPU_H     //��ֹ�ظ����壨CPU_H ��ͷ��
#define CPU_H 

//��1�����䶯����������оƬͷ�ļ�
#include "MKL36Z4.h"
#include "core_cm0plus.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
//��2�����䶯����λ���
#define IS_PIN_RESET_OCCURED    BGET(RCM_SRS0_PIN_SHIFT,RCM_SRS0)    // ������λ
#define IS_POWERON_RESET          BGET(RCM_SRS0_POR_SHIFT, RCM_SRS0)   // �ϵ縴λ
#define CLEAR_PIN_RESET_FLAG         //д1�����Ÿ�λ��־λ

//��3�����̶����жϺ궨��,����ARM�ܹ����򲻱䶯
#define ENABLE_INTERRUPTS        __asm("cpsie i")     // �����ж�
#define DISABLE_INTERRUPTS       __asm("cpsid i")     // �����ж�
//��4�����̶������Ż����ͱ����궨��
typedef volatile uint8_t      vuint8_t;   // ���Ż��޷���8λ�����ֽ�
typedef volatile uint16_t     vuint16_t;  // ���Ż��޷���16λ������
typedef volatile uint32_t     vuint32_t;  // ���Ż��޷���32λ��������
typedef volatile int8_t       vint_8;     // ���Ż��з���8λ��
typedef volatile int16_t      vint_16;    // ���Ż��з���16λ��
typedef volatile int32_t      vint_32;    // ���Ż��з���32λ��
//��5�����̶���λ�����꺯������λ����λ����üĴ���һλ��״̬��
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //�üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //��üĴ���һλ��״̬

#endif    //��ֹ�ظ����壨CPU_H ��β��
