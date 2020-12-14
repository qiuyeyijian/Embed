//============================================================================
//文件名称：dac.c
//功能概要：KL36 DAC底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2020-11-19 V2.0    
//============================================================================

#include "dac.h"  //包含DAC驱动程序头文件   


//================================内部函数====================================
//============================================================================
//函数名称：dac_set_buffer
//函数返回：设置的缓冲区大小值
//参数说明：dacx_base_ptr：DACx基指针      
//       dacindex缓冲区号：0,1
//       buffval缓冲区值:0~4095
//功能概要：设置DACx缓冲区
//============================================================================ 
uint16_t dac_set_buffer(DAC_MemMapPtr dacx_base_ptr, uint8_t dacindex, uint16_t buffval);


//============================================================================
//函数名称：dac_init
//函数返回：无
//参数说明：RefVoltage：参考电压选择 DAC_VREFH或 DAC_VDDA
//功能概要：初始化DAC模块设定
//============================================================================
void dac_init(uint8_t RefVoltage)
{
    SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK;   //使能DAC0时钟

    //配置DAC0_C0寄存器
    if (1 == RefVoltage)
        DAC0_C0 |= DAC_C0_DACRFS_MASK;   //选择VDDA参考电压3.3V 
    else if (0 == RefVoltage)
        DAC0_C0 &= ~DAC_C0_DACRFS_MASK;   //选择VREFH参考电压3.3V      

    DAC0_C0 |= DAC_C0_DACTRGSEL_MASK;     //软件触发
    //软件触发无效，高功耗模式，缓冲区置底中断禁止,缓冲区置顶中断禁止，
    DAC0_C0 &= ~(DAC_C0_DACSWTRG_MASK | DAC_C0_LPEN_MASK | DAC_C0_DACBBIEN_MASK | DAC_C0_DACBTIEN_MASK);

    //配置DAC0_C1寄存器	  
    //DMA禁用 ,正常工作模式
    DAC0_C1 &= ~(DAC_C1_DMAEN_MASK | DAC_C1_DACBFEN_MASK | DAC_C1_DACBFMD_MASK);
    //使能DAC0模块 
    DAC0_C0 |= DAC_C0_DACEN_MASK;
}

//============================================================================
//函数名称：dac_convert
//参数说明：data: 需要转换成模拟量的数字量，范围（0~4095）
//功能概要：执行DAC转换
//============================================================================   
void dac_convert(uint16_t data)
{
    dac_set_buffer(DAC0_BASE_PTR, 0, data);
}

//================================内部函数======================================
//============================================================================
//函数名称：dac_set_buffer
//函数返回：设置的缓冲区大小值
//参数说明：dacx_base_ptr：DACx基指针      
//       dacindex缓冲区号：0,1
//       buffval缓冲区值:0~4095
//功能概要：设置DACx缓冲区
//============================================================================ 
uint16_t dac_set_buffer(DAC_MemMapPtr dacx_base_ptr, uint8_t dacindex, uint16_t buffval)
{
    uint16_t temp = 0;
    //设置缓冲区低字节
    DAC_DATL_REG(dacx_base_ptr, dacindex) = (buffval & 0x0ff);
    //设置缓冲区高字节
    DAC_DATH_REG(dacx_base_ptr, dacindex) = (buffval & 0xf00) >> 8;
    temp = (DAC_DATL_REG(dacx_base_ptr, dacindex) |
        (DAC_DATH_REG(dacx_base_ptr, dacindex) << 8));
    return temp;
}

