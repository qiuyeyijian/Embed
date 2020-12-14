//======================================================================
//文件名称：mcu.h（mcu头文件）
//制作单位：SD-Arm(sumcu.suda.edu.cn)
//更新记录：20181201-20200627
//======================================================================
#ifndef MCU_H     //防止重复定义（MCU_H 开头）
#define MCU_H

//（1）【变动】包含芯片头文件
#include "MKL36Z4.h"
#include "system_MKL36Z4.h"
//（2）【固定】包含cpu头文件
#include "cpu.h"
//（3）【变动】MCU基本信息相关宏常数
//                            "1234567890123456789"
#define MCU_TYPE              "AHL-MKL36Z64-V3.4  "  //MCU型号（19字节）
//                            "123456789"
#define BIOS_TYPE             "20200829 "       //BIOS版本号（9字节）
#define MCU_SECTORSIZE        1024              //扇区大小（B）
#define MCU_SECTOR_NUM        64                //MCU扇区总数
#define MCU_STACKTOP          0x20001800        //栈顶地址
#define MCU_FLASH_ADDR_START  0x00000000        //MCU的FLASH起始地址
#define MCU_FLASH_ADDR_LENGTH 0x00010000        //MCU的FLASH长度（64KB）
#define MCU_RAM_ADDR_START    0x1FFFF800	    //MCU的RAM起始地址
#define MCU_RAM_ADDR_LENGTH   0x00002000	    //MCU的RAM长度（8KB）
#define DisableIRQ(irq)          NVIC_DisableIRQ(irq)       // 禁止irq中断
#define EnableIRQ(irq)           NVIC_EnableIRQ(irq)        // 打开irq中断
#endif    //防止重复定义（MCU_H 结尾）