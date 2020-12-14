//=====================================================================
//文件名称：incapture.c
//功能概要：incapture底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：KL25、KL26、KL36
//=====================================================================

#include "incapture.h"

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//定时器模块0,1,2地址映射
static const TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
static const IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

static uint8_t tpm_mux_val(uint16_t capNo,uint8_t* TPM_i,uint8_t* chl);

static void tpm_timer_init2(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value);

//===================================================================
//函数名称：incap_init
//功能概要：incap模块初始化。
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//        clockFre：时钟频率，单位：Khz，取值：375、750、1500、3000、6000、
//                                                   12000、24000、48000
//        period：周期，单位为个数，即计数器跳动次数，范围为1~65536
//        capmode：输入捕捉模式（上升沿、下降沿、双边沿），有宏定义常数使用
//函数返回：无
//注意：          因为GEC中给出的PWM和输入捕捉都是同一模块的，只是通道不同，所以为防止在使用多组
//          PWM和输入捕捉时，频率篡改，需要使得使用到的clockFre和period参数保持一致。
//===================================================================
void incapture_init(uint16_t capNo,uint32_t clockFre,uint16_t period,uint8_t capmode)
{
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint8_t port,pin;           //解析出的端口、引脚号临时变量
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint8_t mux_val;
    //1. gpio引脚解析
    port = (capNo>>8);     //解析出的端口
    pin = capNo;           //解析出的引脚号
    //2. 根据port，给局部变量port_ptr赋值,获得基地址
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    //3. 取得引脚复用值，并获得解析的tpm模块号和通道号
    mux_val=tpm_mux_val(capNo,&TPM_i,&chl);
    //4. 初始化时钟
    tpm_timer_init2(TPM_i,clockFre,period);
    //4. 根据pin,指定该引脚功能为TPM的通道功能（即令引脚控制寄存器的MUX=mux_val）
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX(mux_val);
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. 输入捕捉参数设定
    if(capmode == CAP_UP)           //上升沿捕捉
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
    else if(capmode == CAP_DOWN)    //下降沿捕捉
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    else if(capmode == CAP_DOUBLE)  //双边沿捕捉
    {
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK;
    }
    //6. chl通道中断使能
    TPM_CnSC_REG(TPM_ARR[TPM_i],chl) |= TPM_CnSC_CHIE_MASK;
    TPM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1);
}

//=====================================================================
//函数名称：incapture_value
//功能概要：获取该通道的计数器当前值
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//函数返回：通道的计数器当前值
//=====================================================================
uint16_t get_incapture_value(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint16_t cnt;
    tpm_mux_val(capNo,&TPM_i,&chl);   //解析tpm模块号和通道号
    cnt=TPM_CnV_REG(TPM_ARR[TPM_i],chl);
    return cnt;
}

//===================================================================
//函数名称：cap_clear_flag
//功能概要：清输入捕捉中断标志位。
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//函数返回：无
//===================================================================
void cap_clear_flag(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    tpm_mux_val(capNo,&TPM_i,&chl);   //解析tpm模块号和通道号
    BSET(TPM_CnSC_CHF_SHIFT,TPM_ARR[TPM_i]->CONTROLS[chl].CnSC);
}

//===================================================================
//函数名称：cap_get_flag
//功能概要：获取输入捕捉中断标志位。
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//函数返回：返回当前中断标志位
//===================================================================
uint8_t cap_get_flag(uint16_t capNo)
{
    uint8_t TPM_i,chl,flag;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    tpm_mux_val(capNo,&TPM_i,&chl);   //解析tpm模块号和通道号
    flag = BGET(TPM_CnSC_CHF_SHIFT,TPM_ARR[TPM_i]->CONTROLS[chl].CnSC);
    return flag;
}

//===================================================================
//函数名称：cap_enable_int
//功能概要：使能输入捕捉中断。
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//函数返回：无
//===================================================================
void cap_enable_int(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    tpm_mux_val(capNo,&TPM_i,&chl);   //解析tpm模块号和通道号
    EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//函数名称：cap_disable_int
//功能概要：禁止输入捕捉中断。
//参数说明：capNo：模块号，使用gec.h中宏定义，例如INCAP_PIN0
//函数返回：无
//===================================================================
void cap_disable_int(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    tpm_mux_val(capNo,&TPM_i,&chl);   //解析tpm模块号和通道号
    DisableIRQ(TPM_IRQ[TPM_i]);
}

//------以下为内部函数------
//=====================================================================
//函数名称：tpm_mux_val
//功能概要：将传进参数tpmx_Chy进行解析，得出具体模块号与通道号（例：TPM_CH0
//        解析出PORT引脚，并根据引脚返回mux_val）。
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//
//函数返回：gpio复用寄存器传入值
//=====================================================================
static uint8_t tpm_mux_val(uint16_t capNo,uint8_t* TPM_i,uint8_t* chl)
{
    uint8_t port,pin;
    //1.解析模块号和通道号
    switch(capNo)
    {
        case ((2<<8)|1):*TPM_i =0;*chl=0;break;
        case ((2<<8)|2):*TPM_i =0;*chl=1;break;
        case ((2<<8)|3):*TPM_i =0;*chl=2;break;
        case ((3<<8)|3):*TPM_i =0;*chl=3;break;
        case ((3<<8)|4):*TPM_i =0;*chl=4;break;
        case ((3<<8)|5):*TPM_i =0;*chl=5;break;
        default:break;
    }
    //2.解析引脚复用寄存器传入值
    port = (capNo>>8);
    pin = capNo;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}

//===================================================================
//函数名称： tpm_timer_init
//功能概要： tpm模块初始化，设置计数器频率f及计数器溢出时间MOD_Value。
//参数说明： TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//        f：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//        MOD_Value：单位：ms，范围取决于计数器频率与计数器位数（16位）
//函数返回： 无
//===================================================================
static void tpm_timer_init2(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value)
{
    //局部变量声明
    uint32_t clk_f,clk_div;
    //（1）开启SIM时钟门
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //（2）开启时钟，默认选择用PLL/2时钟源，即48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    TPM_ARR[TPM_i]->SC = 0;
    //（3） 由期望的时钟频率f，计算分频因子clk_div。因分频系数clk_f=48000/f,
    //则分频因子clk_div=sqrt(clk_f)。例如：f=3000Khz,则clk_f=16，clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //（4） 计数器清零
    TPM_ARR[TPM_i]->CNT = 0;
    //（5）设置模数寄存器
    if(MOD_Value == 0)
    {
    	TPM_ARR[TPM_i]->MOD = 0;  //给模数寄存器赋值
    }
    else
    {
    	TPM_ARR[TPM_i]->MOD = MOD_Value - 1;  //给模数寄存器赋值
    }
    //（6）写TPM_i的状态和控制寄存器
    TPM_ARR[TPM_i]->SC |= TPM_SC_PS(clk_div);
}

