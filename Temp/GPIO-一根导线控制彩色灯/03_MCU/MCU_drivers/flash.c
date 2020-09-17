//===========================================================================
//�ļ����ƣ�flash.c
//���ܸ�Ҫ��Flash�ײ���������Դ�ļ�
//��Ȩ���У�SD-Arm(sumcu.suda.edu.cn)
//�汾���£�20181201-20200221
//оƬ���ͣ�KL36
//===========================================================================
//����ͷ�ļ�
#include "flash.h" 
#include "string.h"//���ú���memcpy�������ͷ�ļ�

//��User������Ӵ�3��
#if(USE_BIOS_FLASH==0)   //1�������̳���BIOS��0����ʹ���Դ�����
//=================�ڲ����ú�������=========================================
//======================================================================
//�������أ�0-�ɹ� 1-ʧ��
//����˵������
//���ܸ�Ҫ������Flash����
//======================================================================
uint32_t flash_cmd_launch(void);
//======================================================================

//=================�ⲿ�ӿں���=============================================
//======================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��flash��ʼ��
//======================================================================
void flash_init(void)
{   
    // �ȴ��������
    while(!(FTFA_FSTAT & CCIF));
    
  // ������ʳ����־λ
    FTFA_FSTAT = ACCERR | FPVIOL;
    //������ģ����ƽ̨���ƼĴ�����PLACR_ESFC��Flashģ���д����
    BSET(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);
    /*
    //������ģ����ƽ̨���ƼĴ�����PLACR_ESFC��Flashģ���д������
    BCLR(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);   //��ʵ��۲������ã�
    */
}

//======================================================================
//�������أ�����ִ��ִ��״̬��0=������1=�쳣��
//����˵����sect��Ŀ�������ţ���Χȡ����ʵ��оƬ����mcu.h��
//���ܸ�Ҫ������flash�洢����sect����
//======================================================================
uint8_t flash_erase(uint16_t sect)
{
    union
    {
        uint32_t  word;
        uint8_t   byte[4];
    } dest;
    
    dest.word    = (uint32_t)(sect*(1<<10));

    // ���ò�������
    FTFA_FCCOB0 = ERSSCR; // ������������
    
    // ����Ŀ���ַ
    FTFA_FCCOB1 = dest.byte[2];
    FTFA_FCCOB2 = dest.byte[1];
    FTFA_FCCOB3 = dest.byte[0];
    
    // ִ����������
    if(1 == flash_cmd_launch())    //��ִ��������ִ���
        return 1;     //�����������
   
    // ������sector0ʱ��������豸
//    if(dest.word <= 0x800)
    if(dest.word < 0x400)
    {
        // д��4�ֽ�
        FTFA_FCCOB0 = PGM4; 
        // ����Ŀ���ַ
        FTFA_FCCOB1 = 0x00;
        FTFA_FCCOB2 = 0x04;
        FTFA_FCCOB3 = 0x0C;
        // ����
        FTFA_FCCOB4 = 0xFF;
        FTFA_FCCOB5 = 0xFF;
        FTFA_FCCOB6 = 0xFF;
        FTFA_FCCOB7 = 0xFE;
        // ִ����������
        if(1 == flash_cmd_launch())  //��ִ��������ִ���
            return 2;   //�����������
    }  
    
    return 0;  //�ɹ�����
}
//======================================================================
//�������أ�����ִ��״̬��0=������1=�쳣��
//����˵����sect�������ţ���Χȡ����ʵ��оƬ����mcu.h��
//        offset:д�������ڲ�ƫ�Ƶ�ַ��Ҫ��Ϊ0,4��8,12��......��
//        N��д���ֽ���Ŀ��Ҫ��Ϊ4��8,12,......��
//        buf��Դ���ݻ������׵�ַ
//���ܸ�Ҫ����buf��ʼ��N�ֽ�д�뵽flash�洢����sect������ offset��
//======================================================================
uint8_t flash_write(uint16_t sect,uint16_t offset,
							uint16_t N,uint8_t *buf)
{
    uint32_t size;
    uint32_t destaddr;
    union
    {
        uint32_t   word;
        uint8_t  byte[4];
    } dest;
    
    if(offset%4 != 0)
        return 1;   //�����趨����ƫ����δ���루4�ֽڶ��룩
    
    // ����д������
    FTFA_FCCOB0 = PGM4;
    destaddr = (uint32_t)(sect*(1<<10) + offset);//�����ַ
    dest.word = destaddr;
    for(size=0; size<N; size+=4, dest.word+=4, buf+=4)
    {
        // ����Ŀ���ַ
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];
 
        // ��������
        FTFA_FCCOB4 = buf[3];//���Ѱַ
        FTFA_FCCOB5 = buf[2];
        FTFA_FCCOB6 = buf[1];
        FTFA_FCCOB7 = buf[0];
        
        if(1 == flash_cmd_launch()) 
            return 2;  //д���������
    }
    
    return 0;  //�ɹ�ִ��
}

//========================================================================
//�������أ���
//����˵����   destaddr��Ŀ���ַ
//        N��д�ֽ���Ŀ��0~1020,Ҫ��Ϊ4��8,12,......��
//        buf��д�����ݴ�Ŵ�
//���ܸ�Ҫ����ȡflashָ����ַ������
//========================================================================
uint8_t flash_write_physical(uint32_t destaddr,uint16_t N,uint8_t *buf)
{
    uint32_t size;
    union
    {
        uint32_t   word;
        uint8_t  byte[4];
    } dest;

    // ����д������
    FTFA_FCCOB0 = PGM4;
    dest.word = destaddr;
    for(size=0; size<N; size+=4, dest.word+=4, buf+=4)
    {
        // ����Ŀ���ַ
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];

        // ��������
        FTFA_FCCOB4 = buf[3];//���Ѱַ
        FTFA_FCCOB5 = buf[2];
        FTFA_FCCOB6 = buf[1];
        FTFA_FCCOB7 = buf[0];

        if(1 == flash_cmd_launch())
            return 2;  //д���������
    }

    return 0;  //�ɹ�ִ��
}

//======================================================================
//�������أ���
//����˵����sect�������ţ���Χȡ����ʵ��оƬ����mcu.h��
//        offset:�����ڲ�ƫ�Ƶ�ַ��Ҫ��Ϊ0,4��8,12��......��
//        N�����ֽ���Ŀ��Ҫ��Ϊ4��8,12,......��
//        dest���������ݴ�Ŵ�������ַ��Ŀ���Ǵ����������ݣ�RAM����
//���ܸ�Ҫ����ȡflash�洢����sect������ offset����ʼ��N�ֽڣ���RAM��dest��
//========================================================================
void flash_read_logic(uint8_t *dest,uint16_t sect,
                                  uint16_t offset,uint16_t N)
{
	uint8_t *src;
	src=(uint8_t *)(sect*1024+offset);
	memcpy(dest,src,N);
}


//======================================================================
//�������أ���
//����˵����dest���������ݴ�Ŵ�������ַ��Ŀ���Ǵ����������ݣ�RAM����
//       addr��Ŀ���ַ��Ҫ��Ϊ4�ı��������磺0x00000004��
//       N�����ֽ���Ŀ��0~1020,Ҫ��Ϊ4��8,12,......��
//���ܸ�Ҫ����ȡflashָ����ַ������
//======================================================================
void flash_read_physical(uint8_t *dest,uint32_t addr,uint16_t N)
{
	uint8_t *src;
	src=(uint8_t *)addr;
	memcpy(dest,src,N);
}

//========================================================================
//�������أ���
//����˵����sect����������������������ֵ
//���ܸ�Ҫ��flash��������
//˵        ����ÿ���ñ�����һ�Σ�����4��������M~M+3��
//========================================================================
void flash_protect(uint16_t sect)
{
    uint8_t regionNO;
    //regionNO=M/4;
    regionNO=sect/2;
    uint8_t offset;
    offset=regionNO%8;//���ƫ�ƣ�������λ��
    uint8_t regionCounter;
    regionCounter=3-regionNO/8;//���Ӧ��λ�ļĴ�����
    switch(regionCounter)
    {
    case 3:   //��������[7��0]��ĳ���򣨶�Ӧ[28:0]��ʼ��4��������4�������룩
    	BCLR(offset,FTFA_FPROT3);
        break;
    case 2:  //��������[15:8]��ĳ���򣨶�Ӧ[60:32]��ʼ��4��������4�������룩
    	BCLR(offset,FTFA_FPROT2);
        break;
    case 1:  //��������[23:16]��ĳ���򣨶�Ӧ[92:64]��ʼ��4��������4�������룩
    	BCLR(offset,FTFA_FPROT1);
        break;
    case 0:  //��������[31:24]��ĳ���򣨶�Ӧ[124:96]��ʼ��4��������4�������룩
    	BCLR(offset,FTFA_FPROT0);
        break;
    default:
        break;
    }
}

//========================================================================
//�������أ�1=Ŀ������Ϊ�գ�0=Ŀ������ǿա�
//����˵������Ҫ̽���flash���������ż��ֽ���
//���ܸ�Ҫ��flash�пղ���
//========================================================================
uint8_t flash_isempty(uint16_t sect,uint16_t N)
{
   uint16_t i,flag;
   uint8_t* buff;
   i = 0;
   flag = 1;
   for(i = 0; i<N; i++)   //�����������ֽ�
   {
	  buff=(uint8_t*)(sect*0x400);
      if(buff[i]!=0xff)   //�ǿ�
      {
      	  flag=0;
          break;
       }
   }
   return flag;
}

//========================================================================
//�������أ���
//����˵������Ҫ�Ᵽ��������
//���ܸ�Ҫ��flash�����Ᵽ������
//========================================================================
void flash_unprotect(uint16_t sect)
{
    uint8_t regionNO;
    regionNO=sect/2;
    uint8_t offset;
    offset=regionNO%8;//���ƫ�ƣ�������λ��
    uint8_t regionCounter;
    regionCounter=3-regionNO/8;//���Ӧ��λ�ļĴ�����
    switch(regionCounter)
    {
    case 3:     //��������[7��0]��ĳ���򣨶�Ӧ[28:0]��ʼ��4��������4�������룩
    	BSET(offset,FTFA_FPROT3);
        break;
    case 2:     //��������[15:8]��ĳ���򣨶�Ӧ[60:32]��ʼ��4��������4�������룩
    	BSET(offset,FTFA_FPROT2);
        break;
    case 1:     //��������[23:16]��ĳ���򣨶�Ӧ[92:64]��ʼ��4��������4�������룩
    	BSET(offset,FTFA_FPROT1);
        break;
    case 0:     //��������[31:24]��ĳ���򣨶�Ӧ[124:96]��ʼ��4��������4�������룩
    	BSET(offset,FTFA_FPROT0);
        break;
    default:
        break;
    }
}

//========================================================================
//�������أ�1=������������0=����δ������
//����˵������Ҫ��������
//���ܸ�Ҫ���ж�flash�����Ƿ񱻱���
//=========================================================================
uint8_t flash_isSectorProtected(uint16_t sect)
{
	 uint8_t regionNO;
	 regionNO=sect/2;
	 uint8_t offset,res;
	 res=0;
	 offset=regionNO%8;//���ƫ�ƣ�������λ��
	 uint8_t regionCounter;
	 regionCounter=3-regionNO/8;//���Ӧ��λ�ļĴ�����
	 switch(regionCounter)
	 {
	  case 3:     //��������[7��0]��ĳ���򣨶�Ӧ[28:0]��ʼ��4��������4�������룩
		  res=BGET(offset,FTFA_FPROT3);
		break;
	  case 2:     //��������[15:8]��ĳ���򣨶�Ӧ[60:32]��ʼ��4��������4�������룩
		  res=BGET(offset,FTFA_FPROT2);
		break;
	  case 1:     //��������[23:16]��ĳ���򣨶�Ӧ[92:64]��ʼ��4��������4�������룩
		  res=BGET(offset,FTFA_FPROT1);
		break;
	  case 0:     //��������[31:24]��ĳ���򣨶�Ӧ[124:96]��ʼ��4��������4�������룩
		  res=BGET(offset,FTFA_FPROT0);
		break;
	  default:
		break;
	}
	 return ~res;
}

//----------------------����Ϊ�ڲ�������Ŵ�---------------------------------
//======================================================================
//�������أ�0-�ɹ� 1-ʧ��
//����˵������
//���ܸ�Ҫ������Flash����
//======================================================================
uint32_t flash_cmd_launch(void)
{
	uint8_t irq_flag=0;
    // ������ʴ����־λ�ͷǷ����ʱ�־λ
    FTFA_FSTAT = ACCERR | FPVIOL;
    if(__get_PRIMASK() == 0U)
    {
    	__disable_irq();  //�����ж�
    	irq_flag = 1;
    }
    // ��������
    FTFA_FSTAT = CCIF;
    // �ȴ��������
    while(!(FTFA_FSTAT & CCIF));
    if(irq_flag == 1)
    {
    	__enable_irq();  //�����ж�
    }
    // �������־
    if(FTFA_FSTAT & (ACCERR | FPVIOL | MGSTAT0))
        return 1 ; //ִ���������
    return 0; //ִ������ɹ�
}
//======================================================================
#endif

