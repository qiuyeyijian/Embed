//=====================================================================
//文件名称：timer.c
//功能概要：timer底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：KL25、KL26、KL36
//=====================================================================
#include "timer.h"

void lptmr_init(uint32_t time_ms);
void lptmr_reset(void);
void lptmr_enable_int(void);
void lptmr_disable_int(void);
void tpm_timer_init(uint16_t TPM_i,uint32_t f,float MOD_Value);
void tpm_enable_int(uint8_t TPM_i);
void tpm_disable_int(uint8_t TPM_i);
uint8_t tpm_get_int(uint8_t TPM_i);
void tpm_clear_int(uint8_t TPM_i);

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//定时器模块0,1,2地址映射
TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

//============================================================================
//函数返回：无
//参数说明： timer_No:时钟模块号  具体时钟模块号见gec.h文件定义
//        time_ms 定时器中断的时间间隔，单位为毫秒，其中timer0设置范围 0~65535ms，
//		  timer1与timer2定时时长范围为0~65535/3ms
//功能概要：时钟模块初始化，其中timer0为低功耗时钟 timer1与timer2为非低功耗时钟
//============================================================================
void timer_init(uint8_t timer_No,uint32_t time_ms)
{
    if(timer_No==0)
    {
        lptmr_init(time_ms);
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_timer_init(timer_No,3000,time_ms);
    }
    else
        return;
}


//============================================================================
//函数返回：无
//参数说明： timer_No:时钟模块号  具体时钟模块号见gec.h文件定义
//功能概要：时钟模块使能，开启时钟模块中断及定时器中断
//============================================================================
void timer_enable_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
        lptmr_enable_int();
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_enable_int(timer_No);
    }
    else
        return;
}

//============================================================================
//函数返回：无
//参数说明： timer_No:时钟模块号  具体时钟模块号见gec.h文件定义
//功能概要：定时器中断除能
//============================================================================
void timer_disable_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
        lptmr_disable_int();
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_disable_int(timer_No);
    }
    else
        return;
}

//===================================================================
//参数说明： timer_No:时钟模块号  具体时钟模块号见gec.h文件定义
//功能概要：获取timer模块中断标志
//函数返回：中断标志 1=有对应模块中断产生;0=无对应模块中断产生
//===================================================================
uint8_t timer_get_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
      //获取TPM_i模块中断标志位
      if((LPTMR0_CSR & LPTMR_CSR_TCF_MASK)==LPTMR_CSR_TCF_MASK)
          return 1;
      else
          return 0;
    }
    else if(timer_No==1||timer_No==2)
    {
        return tpm_get_int(timer_No);
    }
    else
        return 0;
}

//============================================================================
//函数返回：无
//参数说明： timer_No:时钟模块号  具体时钟模块号见gec.h文件定义
//功能概要：定时器清除中断标志
//============================================================================
void timer_clear_int(uint8_t timer_No)
{
    if(timer_No==0)
    {
        CLEAR_LPTMR_FLAG;
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_clear_int(timer_No);
    }
    else
        return;
}

//内部函数
//============================================================================
//函数返回：无
//参数说明：time_ms 定时器中断的时间间隔，单位为毫秒，时间设置范围 0~65535ms
//功能概要：LPTMR模块初始化，时钟源已配置为1KHz的LPO时钟
//其他以此类推
//============================================================================
void lptmr_init(uint32_t time_ms)
{
    uint16_t compare_value;
    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//使能LPTMR模块时钟
    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0x11);//选择LPO时钟
    LPTMR0_PSR |= LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    compare_value=time_ms;
    LPTMR0_CMR = LPTMR_CMR_COMPARE(compare_value);      //设置比较寄存器值
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; //开启LPTMR模块设置
}


//====================内部函数==========================================
//============================================================================
//函数返回：无
//参数说明：无
//功能概要：LPTMR计时器清零
//============================================================================
void lptmr_reset(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;
}

//============================================================================
//函数返回：无
//参数说明：无
//功能概要：LPTMR模块使能，开启LPTMR模块中断及定时器中断
//============================================================================
void lptmr_enable_int(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TIE_MASK;        //开启LPTMR定时器中断
    NVIC_EnableIRQ(LPTMR0_IRQn);           //开引脚的IRQ中断
}

//============================================================================
//函数返回：无
//参数说明：无
//功能概要：LPT定时器中断除能
//============================================================================
void lptmr_disable_int(void)
{
    LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;      //禁止LPTMR定时器中断
    NVIC_DisableIRQ(LPTMR0_IRQn);          //关引脚的IRQ中断
}

//===================================================================
//功能概要： tpm模块初始化，设置计数器频率f及计数器计数间隔时间MOD_Value。
//参数说明： TPM_i：模块号，可用参数可参见gec.h文件
//        f：计数器频率，单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//        MOD_Value：计数器计数间隔时间，单位：ms，(f*MOD_Value-1)的范围为0~65535
//函数返回： 无
//===================================================================
void tpm_timer_init(uint16_t TPM_i,uint32_t f,float MOD_Value)
{
    //局部变量声明
    uint32_t clk_f,clk_div;
    uint16_t mod;
    //1. 开启SIM时钟门
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    TPM_ARR[TPM_i]->SC = 0;   //关闭该模块的计数器功能，确保未重复开启
    //2.开启时钟，默认选择用PLL/2时钟源，即48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    //3. 由期望的时钟频率f，计算分频因子clk_div。因分频系数clk_f=48000/f,
    //则分频因子clk_div=sqrt(clk_f)。例如：f=3000Khz,则clk_f=16，clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //4. 计数器清零
    TPM_ARR[TPM_i]->CNT = 0;
    //5. 设置模数寄存器
    if(f * MOD_Value > 65535)
    {
        mod = 65535;
    }
    else if(f * MOD_Value <= 0)
    {
        mod = 0 ;
    }
    else{
        mod = f * MOD_Value - 1;
    }
    TPM_ARR[TPM_i]->MOD = mod;  //给模数寄存器赋值
    //6. 写TPM_i的状态和控制寄存器
    TPM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(clk_div) | TPM_SC_TOIE_MASK;
}

//===================================================================
//功能概要：使能tpm模块中断。
//参数说明：TPM_i：模块号，可用参数可参见gec.h文件
//函数返回：无
//===================================================================
void tpm_enable_int(uint8_t TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //开TPM中断
    NVIC_EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//功能概要：禁用tpm模块中断。
//参数说明：TPM_i：模块号，可用参数可参见gec.h文件
//函数返回：无
//===================================================================
void tpm_disable_int(uint8_t TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //开TPM中断
    NVIC_DisableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//功能概要：获取TPM模块中断标志
//参数说明：TPM_i：模块号，可用参数可参见gec.h文件
//函数返回：中断标志 1=有对应模块中断产生;0=无对应模块中断产生
//===================================================================
uint8_t tpm_get_int(uint8_t TPM_i)
{
  //获取TPM_i模块中断标志位
  if(((TPM_SC_REG(TPM_ARR[TPM_i]) & TPM_SC_TOF_MASK)==TPM_SC_TOF_MASK))
    return 1;
  else
    return 0;
}

//===================================================================
//功能概要：清除TPM中断标志
//参数说明：TPM_i：模块号，可用参数可参见gec.h文件
//函数返回：清除TPM模块中断标志位
//===================================================================
void tpm_clear_int(uint8_t TPM_i)
{
    //清除TPM_i模块中断标志位
    BSET(TPM_SC_TOF_SHIFT,TPM_SC_REG(TPM_ARR[TPM_i]));
}
