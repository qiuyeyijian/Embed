//=====================================================================
//文件名称：pwm.c
//功能概要：pwm底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：KL25、KL26、KL36
//=====================================================================
#include "pwm.h"

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//定时器模块0,1,2地址映射
TPM_MemMapPtr PWM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};

//*****************************内部函数******************************
static uint8_t tpm_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl);
static void tpm_timer_init1(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value);

//*****************************对外接口函数******************************
//=====================================================================
//函数名称：  pwm_init
//功能概要：  pwm初始化函数
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          clockFre：时钟频率，单位：Khz，取值：375、750、1500、3000、6000、
//                                                   12000、24000、48000
//          period：周期，单位为个数，即计数器跳动次数，范围为1~65536
//          duty：占空比：0.0~100.0对应0%~100%
//          align：对齐方式 ，在头文件宏定义给出，如PWM_EDGE为边沿对其。
//          pol：极性，在头文件宏定义给出，如PWM_PLUS为正极性
//函数返回：  无
//注意：          因为GEC中给出的PWM和输入捕捉都是同一模块的，只是通道不同，所以为防止在使用多组
//          PWM和输入捕捉时，频率篡改，需要使得使用到的clockFre和period参数保持一致。
//     占空比：是指高电平在一个周期中所占的时间比例。
//=====================================================================
void pwm_init(uint16_t pwmNo,uint32_t clockFre,uint16_t period,float duty,
		                                        uint8_t align,uint8_t pol)
{
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint8_t port,pin;           //解析出的端口、引脚号临时变量
    uint8_t mux_val;            //用于存放获取的复用制
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint32_t temp;
    // 防止越界
    if(duty>100.0) duty=100.0;
    //（1）gpio引脚解析
    port = (pwmNo>>8);          //解析出的端口
    pin = pwmNo;                //解析出的引脚号
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    //（2）取得引脚复用值，并获得解析的tpm模块号和通道号
    mux_val=tpm_mux_val(pwmNo,&TPM_i,&chl);
    //（3）根据pin,指定该引脚功能为TPM的通道功能（即令引脚控制寄存器的MUX=mux_val）
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK;
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. PWM对齐方式的设定
    if(align == PWM_CENTER)         //中心对齐
    {
        tpm_timer_init1(TPM_i,clockFre,period / 2);
    	PWM_ARR[TPM_i]->SC |= TPM_SC_CPWMS_MASK;   //设置中心对齐
        TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0;
        //6. PWM极性的设定和PWM占空比的设定
        if(pol == PWM_PLUS)            //正极性
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSB_MASK;
            temp = (uint32_t)(duty*period/200);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
        else                            //负极性
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSA_MASK;
            temp = (uint32_t)(period*(100-duty)/200);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
    }
    else                            //边沿对齐
    {
        tpm_timer_init1(TPM_i,clockFre,period);
    	PWM_ARR[TPM_i]->SC &= ~TPM_SC_CPWMS_MASK; //设置边沿对齐
    	TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0;     //清通道控制寄存器
        //6. PWM极性的设定和PWM占空比的设定
        temp = PWM_ARR[TPM_i]->MOD + 1;    //计算周期（period）
        if(pol == PWM_PLUS)             //正极性
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSB_MASK;
            temp = (uint32_t)(temp*duty/100);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
        else                            //负极性
        {
        	TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0; //清通道控制状态寄存器
        	//设置负极性
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSA_MASK;
            //设置占空比
            temp = (uint32_t)(temp*(100-duty)/100);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
    }
    PWM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1);//使能计数器
}

//=====================================================================
//函数名称：  pwm_update
//功能概要：  tpmx模块Chy通道的PWM更新
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          duty：占空比：0.0~100.0对应0%~100%
//函数返回：  无
//=====================================================================
void pwm_update(uint16_t pwmNo,float duty)
{
    uint8_t TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint32_t period;
    // 防止越界
    if(duty>100.0)  duty=100.0;
    //1. 取得引脚复用值，并获得解析的tpm模块号和通道号
    tpm_mux_val(pwmNo,&TPM_i,&chl);
    //2. 更新PWM通道寄存器值
    period=PWM_ARR[TPM_i]->MOD;
    TPM_CnV_REG(PWM_ARR[TPM_i],chl)=(uint32_t)(period*duty/100);
}

//------以下为内部函数------
//=====================================================================
//函数名称：tpm_mux_val
//功能概要：将传进参数pwmNo进行解析，得出具体模块号与通道号（例：PWM_PIN0
//         解析出PORT引脚，并根据引脚返回mux_val）。
//参数说明：pwmNo：pwm模块号，在头文件的宏定义给出，如PWM_PIN0表示PWM0通道
//
//函数返回：gpio复用寄存器传入值
//=====================================================================
static uint8_t tpm_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl)
{
    uint8_t port,pin;
    //1.解析模块号和通道号
    switch(pwmNo)
    {
		case ((2<<8)|1):*TPM_i =0;*chl=0;break;
		case ((2<<8)|2):*TPM_i =0;*chl=1;break;
		case ((2<<8)|3):*TPM_i =0;*chl=2;break;
		case ((3<<8)|3):*TPM_i =0;*chl=3;break;
		case ((3<<8)|4):*TPM_i =0;*chl=4;break;
		case ((3<<8)|5):*TPM_i =0;*chl=5;break;
        default: break;
    }
    //2.解析引脚复用寄存器传入值
    port = (pwmNo>>8);
    pin = pwmNo;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}

//===================================================================
//函数名称：  tpm_timer_init
//功能概要：  tpm模块初始化，设置计数器频率f及计数器溢出时间MOD_Value。
//参数说明：  pwmNo：pwm模块号，在gec.h的宏定义给出，如PWM_PIN0表示PWM0通道
//          f：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//          MOD_Value：单位个数：范围取决于计数器频率与计数器位数（16位）
//函数返回：  无
//===================================================================
static void tpm_timer_init1(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value)
{
    //局部变量声明
    uint32_t clk_f,clk_div;
    //（1） 开启SIM时钟门
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //（2）开启时钟，默认选择用PLL/2时钟源，即48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    PWM_ARR[TPM_i]->SC = 0;
    //（3） 由期望的时钟频率f，计算分频因子clk_div。因分频系数clk_f=48000/f,
    //则分频因子clk_div=sqrt(clk_f)。例如：f=3000Khz,则clk_f=16，clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //（4）计数器清零
    PWM_ARR[TPM_i]->CNT = 0;
    //（5）设置模数寄存器
    if(MOD_Value == 0)
    {
    	PWM_ARR[TPM_i]->MOD = 0;  //给模数寄存器赋值
    }
    else
    {
    	PWM_ARR[TPM_i]->MOD = MOD_Value - 1;  //给模数寄存器赋值
    }
    //（6）写TPM_i的状态和控制寄存器
    PWM_ARR[TPM_i]->SC |=  TPM_SC_PS(clk_div) | TPM_SC_TOIE_MASK;
}
