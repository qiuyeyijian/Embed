//======================================================================
//�ļ����ƣ�uart.c
//���ܸ�Ҫ��uart�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//���¼�¼��2017-04-19 V1.0
//======================================================================
#include "uart.h"

//=====����1��2�ŵ�ַӳ��====
const UART_MemMapPtr UART_ARR[] = {UART1_BASE_PTR, UART2_BASE_PTR};
//====���崮��IRQ�Ŷ�Ӧ��====
IRQn_Type table_irq_uart[3] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};

//�ڲ���������
uint8_t uart_is_uartNo(uint8_t uartNo);

//======================================================================
//���ܸ�Ҫ����ʼ��uartģ��                                                
//����˵����uartNo:���ںţ�UART_0��UART_1��UART_2 
//          baud:�����ʣ�300��600��1200��2400��4800��9600��19200��115200...
//�������أ���
//======================================================================
void uart_init(uint8_t uartNo, uint32_t baud_rate)
{
    //�ֲ���������
    uint16_t sbr;
    uint8_t temp;
    UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR;//UART_2h_0ΪUART0_MemMapPtr����ָ��
    UART_MemMapPtr UART_2h_1_2;    //UART_2h_1_2ΪUART_MemMapPtr����ָ��

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    //���ݴ������uartNo�����ֲ�����UART_2h_0��UART_2h_1_2��ֵ
    if(uartNo==0)
    {
        //UART0ѡ��MCGFLLCLK_khz=48000Khzʱ��Դ
        SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
#ifdef UART_0_GROUP
        //����ѡ�����ö�Ӧ����ΪUART0
        switch(UART_0_GROUP)
        {
        case 0:
            PORTE_PCR20 |= PORT_PCR_MUX(0x4);    //ʹ��UART0_TXD
            PORTE_PCR21 |= PORT_PCR_MUX(0x4);    //ʹ��UART0_RXD
            break;
        case 1:
            PORTA_PCR2 |= PORT_PCR_MUX(0x2);     //ʹ��UART0_TXD
            PORTA_PCR1 |= PORT_PCR_MUX(0x2);     //ʹ��UART0_RXD
            break;
        case 2:
        	PORTB_PCR17 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_TXD
        	PORTB_PCR16 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_RXD
            break;
        case 3:
        default:
            PORTD_PCR7 |= PORT_PCR_MUX(0x3);     //ʹ��UART0_TXD
            PORTD_PCR6 |= PORT_PCR_MUX(0x3);     //ʹ��UART0_RXD
            break;
        }
#endif
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;    //��������0ʱ��

        //��ʱ�رմ���0��������չ���
        UART0_C2_REG(UART_2h_0) &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

        //���ô��ڹ���ģʽ:8λ��У��ģʽ
        sbr = (uint16_t)((SystemCoreClock)/(baud_rate * 16));
        temp = UART0_BDH_REG(UART_2h_0) & ~(UART0_BDH_SBR(0x1F));
        UART0_BDH_REG(UART_2h_0) = temp | UART0_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART0_BDL_REG(UART_2h_0) = (uint8_t)(sbr & UART0_BDL_SBR_MASK);

        //��ʼ�����ƼĴ��������־λ
        UART0_C4_REG(UART_2h_0) = 0x0F;
        UART0_C1_REG(UART_2h_0) = 0x00;
        UART0_C3_REG(UART_2h_0) = 0x00;
        UART0_MA1_REG(UART_2h_0) = 0x00;
        UART0_MA2_REG(UART_2h_0) = 0x00;
        UART0_S1_REG(UART_2h_0) |= 0x1F;
        UART0_S2_REG(UART_2h_0) |= 0xC0;
        //���ô��ڷ����ж�
        UART0_C2_REG(UART_2h_0) &= ~UART0_C2_TIE_MASK;
        //�������ͽ���
        UART0_C2_REG(UART_2h_0) |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    }
    else
    {
    	switch (uartNo)
    	{
    	case UART_1:
#ifdef UART_1_GROUP
    		switch(UART_1_GROUP)
    		{
    		case 0:
    			PORTE_PCR0 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
    			PORTE_PCR1 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
    			break;
    		case 1:
    			PORTA_PCR19 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
    			PORTA_PCR18 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
    			break;
    		case 3:
    		default:
    			PORTC_PCR4 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
    			PORTC_PCR3 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
    			break;
    		}
#endif
    		SIM_SCGC4 |= SIM_SCGC4_UART1_MASK; //��������1ʱ��
    		break;
    		case UART_2:
#ifdef UART_2_GROUP
    			switch(UART_2_GROUP)
    			{
    			case 0:
    				PORTE_PCR16 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
    				PORTE_PCR17 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
    				break;
    			case 1:
    				PORTE_PCR22 = PORT_PCR_MUX(0x4); //ʹ��UART2_TXD
    				PORTE_PCR23 = PORT_PCR_MUX(0x4); //ʹ��UART2_RXD
    				break;
    			case 2:
    				PORTD_PCR3 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
    				PORTD_PCR2 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
    			case 3:
    			default:
    				PORTD_PCR5 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
    				PORTD_PCR4 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
    				break;
    			}
#endif
    			SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;//��������2ʱ��
    			break;
    			default:
    				break;  //���δ��󣬷���
    	}
        UART_2h_1_2 = UART_ARR[uartNo-1]; //���UART1��2ģ����Ӧ�ڻ���ַ

        //��ʱ�رմ���1��2��������չ���
        UART_C2_REG(UART_2h_1_2) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

        //���ò�����,KL25����1��2ʱ��Ƶ��ʹ�� Bus clock = 24M ����ʱ��
        //���ô��ڹ���ģʽ,8λ��У��ģʽ
        //sbr = (uint16_t)((800*1000)/(baud_rate * 16));
        sbr = (uint16_t)((SystemCoreClock/2)/(baud_rate * 16));
        temp = UART_BDH_REG(UART_2h_1_2) & ~(UART_BDH_SBR(0x1F));
        UART_BDH_REG(UART_2h_1_2) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART_BDL_REG(UART_2h_1_2) = (uint8_t)(sbr & UART_BDL_SBR_MASK);

        //��ʼ�����ƼĴ��������־λ
        UART_C1_REG(UART_2h_1_2)=0x00;
        UART_C3_REG(UART_2h_1_2)=0x00;
        UART_S2_REG(UART_2h_1_2)=0x00;
        //���ô��ڷ����ж�
        UART_C2_REG(UART_2h_1_2) &= ~UART_C2_TIE_MASK;
        //�������ͽ���
        UART_C2_REG(UART_2h_1_2)|= (UART_C2_TE_MASK | UART_C2_RE_MASK);
    }
}

//======================================================================
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//          ch:Ҫ���͵��ֽ�
//�������أ�����ִ��״̬��1=���ͳɹ���0=����ʧ�ܡ�
//���ܸ�Ҫ�����з���1���ֽ�
//======================================================================
uint8_t uart_send1(uint8_t uartNo, uint8_t ch)
{
    uint32_t t;
    uint32_t i;
    UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr UART_2h_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ
    /*
    for(i=0;i<5000;i++)
    {
    	__asm (" nop ");
    }
    */
    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//��ѯָ������
    {
        if(0==uartNo)     //�ж�ʹ�õ��ĸ�����
        {
        	//���ͻ�����Ϊ����������
            if ( UART0_S1_REG(UART_2h_0) & UART0_S1_TDRE_MASK )
            {
                UART0_D_REG(UART_2h_0) = ch;
                break;
            }
        }
        else
        {
        	//���ͻ�����Ϊ����������
            if (UART_S1_REG(UART_2h_1_2) & UART_S1_TDRE_MASK )
            {
                UART_D_REG(UART_2h_1_2) = ch;
                break;
            }
        }
    }//end for
    /*
    for(i=0;i<5000;i++)
    {
    	__asm (" nop ");
    }
    */
    if (t >= 0xFBBB)
        return 0; //���ͳ�ʱ������ʧ��
    else
        return 1; //�ɹ�����
}

//======================================================================
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//         buff: ���ͻ�����
//         len:���ͳ���
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�   
//======================================================================
uint8_t uart_sendN(uint8_t uartNo ,uint16_t len ,uint8_t* buff)
{
    uint16_t i;

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (i = 0; i < len; i++)
    {
        if (!uart_send1(uartNo, buff[i])) //����һ���ֽ����ݣ�ʧ��������ѭ��
        {
            break;
        }
    }
    if(i<len)
        return 0;    //���ͳ���
    else
        return 1;    //���ͳ���
}

//======================================================================
//����˵����uartNo:UARTģ���:UART_0��UART_1��UART_2 
//          buff:Ҫ���͵��ַ������׵�ַ
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ����ָ��UART�˿ڷ���һ����'\0'�������ַ���
//======================================================================
uint8_t uart_send_string(uint8_t uartNo, void *buff)
{
    uint16_t i = 0;
    uint8_t *buff_ptr = (uint8_t *)buff;     //����ָ��ָ��Ҫ�����ַ����׵�ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for(i = 0; buff_ptr[i] != '\0'; i++)   //�����ַ�������ַ�
    {
        if (!uart_send1(uartNo,buff_ptr[i]))//����ָ���Ӧ���ַ�
            return 0;  //����ʧ��,����
    }
    return 1;          //���ͳɹ�
}

//======================================================================
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//        *fp:���ճɹ���־��ָ��:*fp=1:���ճɹ���*fp=0:����ʧ��
//�������أ����շ����ֽ�
//���ܸ�Ҫ�����н���1���ֽ�
//======================================================================
uint8_t uart_re1(uint8_t uartNo,uint8_t *fp)
{
    uint32_t t;
    uint8_t  dat;
    UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr UART_2h_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        *fp=0;
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//��ѯָ������
    {
        if(0==uartNo)  //�ж�ʹ�õ��ĸ�����
        {
        //�жϽ��ջ������Ƿ���
        if (UART0_S1_REG(UART_2h_0) & UART0_S1_RDRF_MASK)
            {
                dat=UART0_D_REG(UART_2h_0);    //��ȡ����,������ж�λ
                *fp = 1;  //���ճɹ�
                break;
            }
        }
        else
        {
            //�жϽ��ջ������Ƿ���
            if(UART_S1_REG(UART_2h_1_2) & UART_S1_RDRF_MASK)
            {
                dat=UART_D_REG(UART_2h_1_2);    //��ȡ����,������ж�λ
                *fp= 1;  //���ճɹ�
                break;
            }
        }
    }//end for
    if(t >= 0xFBBB) 
    {
        dat = 0xFF; 
        *fp = 0;    //δ�յ�����
    }
    return dat;    //���ؽ��յ�������
}

//======================================================================
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2                                                   
//          buff: ���ջ�����
//          len:���ճ���
//�������أ�����ִ��״̬ 1=���ճɹ�;0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�,����buff��
//======================================================================
uint8_t uart_reN(uint8_t uartNo ,uint16_t len ,uint8_t* buff)
{
    uint16_t i;
    uint8_t flag = 1;

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    //�ж��Ƿ��ܽ�������
    for (i = 0; i < len && flag==1; i++)
    {
        buff[i] = uart_re1(uartNo, &flag); //��������
    }
    if (i < len)
        return 0; //����ʧ��
    else
        return 1; //���ճɹ�
}

//======================================================================
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ�������ڽ����ж�
//======================================================================
void uart_enable_re_int(uint8_t uartNo)
{
    UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr UART_2h_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0 == uartNo)
        UART0_C2_REG(UART_2h_0) |= UART0_C2_RIE_MASK;    //����UART�����ж�
    else 
        UART_C2_REG(UART_2h_1_2) |= UART_C2_RIE_MASK;    //����UART�����ж�
    NVIC_EnableIRQ(table_irq_uart[uartNo]);    //���жϿ�����IRQ�ж�
}

//======================================================================
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ���ش��ڽ����ж�
//======================================================================
void uart_disable_re_int(uint8_t uartNo)
{
    UART0_MemMapPtr UART_2h_0 = UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr UART_2h_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0==uartNo)
        UART0_C2_REG(UART_2h_0) &= ~UART0_C2_RIE_MASK;    //��ֹUART�����ж�
    else
        UART_C2_REG(UART_2h_1_2) &= ~UART_C2_RIE_MASK;    //��ֹUART�����ж�
    NVIC_DisableIRQ(table_irq_uart[uartNo]);
}

//======================================================================
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2
//�������أ������жϱ�־ 1=�н����ж�;0=�޽����ж�
//���ܸ�Ҫ����ȡ���ڽ����жϱ�־,ͬʱ���÷����ж�
//======================================================================
uint8_t uart_get_re_int(uint8_t uartNo)
{
    uint8_t flag;
    UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr UART_2h_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    if(0==uartNo)
    {
        //���ô��ڷ����жϣ���ֹ���ж�
        UART0_C2_REG(UART_2h_0) &= (~UART0_C2_TIE_MASK);
        //��ȡ�����жϱ�־����ͬʱ�ж�RDRF��RIE
        flag= UART0_S1_REG(UART_2h_0) & (UART0_S1_RDRF_MASK);
        return(BGET(UART0_S1_RDRF_SHIFT,flag)
                & BGET(UART0_C2_RIE_SHIFT,UART0_C2_REG(UART_2h_0)));
    }
    else
    {
        //���ô��ڷ����жϣ���ֹ���ж�
        UART_C2_REG(UART_2h_1_2) &= (~UART_C2_TIE_MASK);
        //��ȡ�����жϱ�־����ͬʱ�ж�RDRF��RIE
        flag= UART_S1_REG(UART_2h_1_2) & (UART_S1_RDRF_MASK);
        return(BGET(UART_S1_RDRF_SHIFT,flag)
                & BGET(UART_C2_RIE_SHIFT,UART_C2_REG(UART_2h_1_2)));
    }
}

//======================================================================
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2
//�������أ���
//���ܸ�Ҫ��uart����ʼ�����رմ���ʱ��
//======================================================================
void uart_deinit(uint8_t uartNo)
{
	UART0_MemMapPtr UART_2h_0=UART0_BASE_PTR;//UART_2h_0ΪUART0_MemMapPtr����ָ��
	UART_MemMapPtr UART_2h_1_2;    //UART_2h_1_2ΪUART_MemMapPtr����ָ��

	switch(uartNo)
	{
	case UART_0:
#ifdef UART_0_GROUP
		switch(UART_0_GROUP)
		{
		case 0:
			PORTE_PCR20 |= PORT_PCR_MUX(0x0);    //ʹ��UART0_TXD
			PORTE_PCR21 |= PORT_PCR_MUX(0x0);    //ʹ��UART0_RXD
			break;
		case 1:
			PORTA_PCR2 |= PORT_PCR_MUX(0x0);     //ʹ��UART0_TXD
			PORTA_PCR1 |= PORT_PCR_MUX(0x0);     //ʹ��UART0_RXD
			break;
		case 2:
			PORTB_PCR17 |= PORT_PCR_MUX(0x0);    //ʹ��UART0_TXD
			PORTB_PCR16 |= PORT_PCR_MUX(0x0);    //ʹ��UART0_RXD
			break;
		case 3:
		default:
			PORTD_PCR7 |= PORT_PCR_MUX(0x0);     //ʹ��UART0_TXD
			PORTD_PCR6 |= PORT_PCR_MUX(0x0);     //ʹ��UART0_RXD
			break;
		}
#endif
		SIM_SCGC4 &= ~SIM_SCGC4_UART0_MASK;    //�رմ���0ʱ��

		break;
	case UART_1:
#ifdef UART_1_GROUP
		switch(UART_1_GROUP)
		{
		case 0:
			PORTE_PCR0 = PORT_PCR_MUX(0x0);    //ʹ��UART1_TXD
			PORTE_PCR1 = PORT_PCR_MUX(0x0);    //ʹ��UART1_RXD
			break;
		case 1:
			PORTA_PCR19 = PORT_PCR_MUX(0x0);    //ʹ��UART1_TXD
			PORTA_PCR18 = PORT_PCR_MUX(0x0);    //ʹ��UART1_RXD
			break;
		case 3:
		default:
			PORTC_PCR4 = PORT_PCR_MUX(0x0);    //ʹ��UART1_TXD
			PORTC_PCR3 = PORT_PCR_MUX(0x0);    //ʹ��UART1_RXD
			break;
		}
#endif
		SIM_SCGC4 &= ~SIM_SCGC4_UART1_MASK; //�رմ���1ʱ��

		break;
	case UART_2:
#ifdef UART_2_GROUP
		switch(UART_2_GROUP)
		{
		case 0:
			PORTE_PCR16 = PORT_PCR_MUX(0x0); //ʹ��UART2_TXD
			PORTE_PCR17 = PORT_PCR_MUX(0x0); //ʹ��UART2_RXD
			break;
		case 1:
			PORTE_PCR22 = PORT_PCR_MUX(0x0); //ʹ��UART2_TXD
			PORTE_PCR23 = PORT_PCR_MUX(0x0); //ʹ��UART2_RXD
			break;
		case 2:
			PORTD_PCR3 = PORT_PCR_MUX(0x0); //ʹ��UART2_TXD
			PORTD_PCR2 = PORT_PCR_MUX(0x0); //ʹ��UART2_RXD
		case 3:
		default:
			PORTD_PCR5 = PORT_PCR_MUX(0x0); //ʹ��UART2_TXD
			PORTD_PCR4 = PORT_PCR_MUX(0x0); //ʹ��UART2_RXD
			break;
		}
#endif
		SIM_SCGC4 &= ~SIM_SCGC4_UART2_MASK;  //�رմ���2ʱ��

		break;
	default:
		break;  //���δ��󣬷���
	}
}

//----------------------����Ϊ�ڲ�������Ŵ�--------------------------------------
//=====================================================================
//�������أ�1:���ں��ں���Χ�ڣ�0�����ںŲ�����
//����˵�������ں�uartNo  :UART_0��UART_1��UART_2
//���ܸ�Ҫ��Ϊ����׳�Զ��ж�uartNo�Ƿ��ڴ������ַ�Χ��
//=====================================================================
uint8_t uart_is_uartNo(uint8_t uartNo)
{
    if(uartNo < UART_0 || uartNo > UART_2)
        return 0;
    else
        return 1;
}
//----------------------------�ڲ���������--------------------------------------
