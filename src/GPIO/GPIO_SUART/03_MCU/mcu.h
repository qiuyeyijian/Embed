//======================================================================
//�ļ����ƣ�mcu.h��mcuͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200627
//======================================================================
#ifndef MCU_H     //��ֹ�ظ����壨MCU_H ��ͷ��
#define MCU_H

//��1�����䶯������оƬͷ�ļ�
#include "MKL36Z4.h"
#include "system_MKL36Z4.h"
//��2�����̶�������cpuͷ�ļ�
#include "cpu.h"
//��3�����䶯��MCU������Ϣ��غ곣��
//                            "1234567890123456789"
#define MCU_TYPE              "AHL-MKL36Z64-V3.4  "  //MCU�ͺţ�19�ֽڣ�
//                            "123456789"
#define BIOS_TYPE             "20200829 "       //BIOS�汾�ţ�9�ֽڣ�
#define MCU_SECTORSIZE        1024              //������С��B��
#define MCU_SECTOR_NUM        64                //MCU��������
#define MCU_STACKTOP          0x20001800        //ջ����ַ
#define MCU_FLASH_ADDR_START  0x00000000        //MCU��FLASH��ʼ��ַ
#define MCU_FLASH_ADDR_LENGTH 0x00010000        //MCU��FLASH���ȣ�64KB��
#define MCU_RAM_ADDR_START    0x1FFFF800	    //MCU��RAM��ʼ��ַ
#define MCU_RAM_ADDR_LENGTH   0x00002000	    //MCU��RAM���ȣ�8KB��
#define DisableIRQ(irq)          NVIC_DisableIRQ(irq)       // ��ֹirq�ж�
#define EnableIRQ(irq)           NVIC_EnableIRQ(irq)        // ��irq�ж�
#endif    //��ֹ�ظ����壨MCU_H ��β��