```c
//====================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：20170801-20200502
//功能描述：见本工程的<01_Doc>文件夹下Readme.txt文件
//====================================================================

#define GLOBLE_VAR
#include "includes.h" //包含总头文件

//----------------------------------------------------------------------
//声明使用到的内部函数
//main.c使用的内部函数声明处

//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    //1. 找到端口相应的基地址，可以参考KL36参考手册
    volatile uint32_t *portA_base = (uint32_t *)0x40049000u;
    volatile uint32_t *portC_base = (uint32_t *)0x4004B000u;

    //2. 找到端口相应引脚控制器的地址
    volatile uint32_t *portA_pcr_5 = portA_base + 5;   //红灯
    volatile uint32_t *portA_pcr_12 = portA_base + 12; //绿灯
    volatile uint32_t *portA_pcr_13 = portA_base + 13; //蓝灯
    volatile uint32_t *portC_pcr_0 = portC_base + 0;
    volatile uint32_t *portC_pcr_1 = portC_base + 1;

    //3. 将端口的相应引脚设置成 GPIO
    *portA_pcr_5 &= ~0x700u; //清MUX位段
    *portA_pcr_5 |= 0x100u;
    *portA_pcr_12 &= ~0x700u; //清MUX位段
    *portA_pcr_12 |= 0x100u;
    *portA_pcr_13 &= ~0x700u; //清MUX位段
    *portA_pcr_13 |= 0x100u;
    *portC_pcr_0 &= ~0x700u; //清MUX位段
    *portC_pcr_0 |= 0x100u;
    *portC_pcr_1 &= ~0x700u; //清MUX位段
    *portC_pcr_1 |= 0x100u;

    //4. 找到端口相应GPIO寄存器的基地址
    volatile uint32_t *gpioA_base = (uint32_t *)0x400FF000u;
    volatile uint32_t *gpioC_base = (uint32_t *)0x400FF080u;

    //6. 找到端口的 GPIO数据输出寄存器 pdor
    volatile uint32_t *portA_pdor = gpioA_base + 0;
    volatile uint32_t *portC_pdor = gpioC_base + 0;

    //7. 找到端口的 GPIO数据输入寄存器 pdir
    volatile uint32_t *portA_pdir = gpioA_base + 4;
    volatile uint32_t *portC_pdir = gpioC_base + 4;

    //8. 找到端口的GPIO数据方向寄存器地址
    volatile uint32_t *portA_pddr = gpioA_base + 5;
    volatile uint32_t *portC_pddr = gpioC_base + 5;

    //9. 定义相应引脚的输入输出状态
    *portA_pddr |= (1 << 5);
    *portA_pddr |= (1 << 12);
    *portA_pddr |= (1 << 13);
    *portC_pddr &= ~(3 << 0);

    //================================================================
    // 对灯的状态进行初始化
    *portA_pdor |= (1 << 5);
    *portA_pdor |= (1 << 12);
    *portA_pdor |= (1 << 13);

    //【不变】开总中断
    ENABLE_INTERRUPTS;

    uint32_t mMainLoopCount; //主循环使用的记录主循环次数变量

    for (;;) //for(;;)（开头）
    {
        // 主循环次数+1，并判断是否小于特定常数
        mMainLoopCount++; //+1
        if (mMainLoopCount <= 13113354)
            continue;       //如果小于特定常数，继续循环
        mMainLoopCount = 0; //清主循环次数

        // 取相应引脚的输入状态
        volatile uint8_t portC_pin_0_state = (*portC_pdir >> 0) & 1;
        volatile uint8_t portC_pin_1_state = (*portC_pdir >> 1) & 1;

        printf("\nportC_pin_0: %d", portC_pin_0_state);
        printf("\nportC_pin_1: %d", portC_pin_1_state);

        // volatile uint8_t flag = (portC_pin_1_state << 1) | portC_pin_1_state;
        volatile uint8_t flag = portC_pin_0_state + portC_pin_1_state;
        printf("\nflag: %d", flag);

        switch (flag)
        {
        case 0:
            *portA_pdor |= (1 << 12);
            *portA_pdor |= (1 << 13);
            *portA_pdor &= ~(1 << 5); //红灯亮

            break;
        case 1:
            *portA_pdor |= (1 << 5);
            *portA_pdor |= (1 << 13);
            *portA_pdor &= ~(1 << 12); //绿灯亮
            printf("case 1");
            break;

        case 2:
            *portA_pdor |= (1 << 5);
            *portA_pdor |= (1 << 12);
            *portA_pdor &= ~(1 << 13); //蓝灯亮
            break;
        default:
            *portA_pdor |= (1 << 5);
            *portA_pdor |= (1 << 12);
            *portA_pdor &= ~(1 << 13); //绿灯亮
            printf("default");
            break;
        }

    } //for(;;)结尾
}
```