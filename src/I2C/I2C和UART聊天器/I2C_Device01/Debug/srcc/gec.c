//=====================================================================
//文件名称：gec.c文件
//制作单位：SD-Arm(sumcu.suda.edu.cn)
//更新记录：20181201-20200627
//移植规则：【固定】
//=====================================================================
#include "gec.h"
//======================================================================
//函数返回：无
//参数说明：无
//功能概要：User对BIOS中断向量表的部分继承,构件库函数指针初始化
//修改信息：WYH，20200805，规范
//======================================================================
void  Vectors_Init()
{
    //有用户程序，编译本段代码
    #if (GEC_USER_SECTOR_START!=0)   
    //（1）若Flash倒数2扇区的前24字节为空，则写入设备序列号及软件版本号初值
    if(flash_isempty(MCU_SECTOR_NUM-1,24))
    {
        flash_write_physical((MCU_SECTOR_NUM-1)*MCU_SECTORSIZE+
        MCU_FLASH_ADDR_START,24,(uint8_t *)"0123456789ABCDEF20200716");
    }
    //（2）继承BIOS的串口中断              
    void ** bios=MCU_FLASH_ADDR_START;   //向量表起始地址  
    uint32_t user[MCU_SECTORSIZE/4];     //向量表变量数组
    //读取USER中断向量表各中断处理程序地址赋给user数组
    flash_read_physical((uint8_t *)user,GEC_USER_SECTOR_START*
    MCU_SECTORSIZE+MCU_FLASH_ADDR_START,MCU_SECTORSIZE);
    //判断USER中串口更新中断是否为BIOS对应中断地址。若是，则不作操作；否则修改
    //为BIOS对应中断地址
    if(user[BIOS_UART_UPDATE_IRQn] != (uint32_t)bios[BIOS_UART_UPDATE_IRQn])
    {
        //改变User中断向量表中BIOS_UART中断向量
        flash_erase(GEC_USER_SECTOR_START);
        //改USER中断向量表的BIOS程序写入串口的中断处理程序地址
        user[2]=(uint32_t)bios[2];     //【20200903】
        user[BIOS_UART_UPDATE_IRQn]=(uint32_t)bios[BIOS_UART_UPDATE_IRQn];
        flash_write_physical(GEC_USER_SECTOR_START*MCU_SECTORSIZE+
        MCU_FLASH_ADDR_START,MCU_SECTORSIZE,(uint8_t *)user);
    }
    
    //（3）中断向量表指针重定向到USER程序的中断向量表
    SCB->VTOR = (uint32_t)(GEC_USER_SECTOR_START*MCU_SECTORSIZE+
    MCU_FLASH_ADDR_START);  
    //（4）给component_fun赋值，SYSTEM_FUNCTION函数用
    component_fun=(void **)(MCU_FLASH_ADDR_START+
    GEC_COMPONENT_LST_START*MCU_SECTORSIZE); 
    //（5）printf提示             
    printf("  【User提示】：将进入User的main()执行...\r\n");
    
    #endif
}

