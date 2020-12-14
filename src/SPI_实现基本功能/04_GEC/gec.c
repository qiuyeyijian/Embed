//=====================================================================
//�ļ����ƣ�gec.c�ļ�
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200627
//��ֲ���򣺡��̶���
//=====================================================================
#include "gec.h"
//======================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��User��BIOS�ж�������Ĳ��ּ̳�,�����⺯��ָ���ʼ��
//�޸���Ϣ��WYH��20200805���淶
//======================================================================
void  Vectors_Init()
{
    //���û����򣬱��뱾�δ���
    #if (GEC_USER_SECTOR_START!=0)   
    //��1����Flash����2������ǰ24�ֽ�Ϊ�գ���д���豸���кż�����汾�ų�ֵ
    if(flash_isempty(MCU_SECTOR_NUM-1,24))
    {
        flash_write_physical((MCU_SECTOR_NUM-1)*MCU_SECTORSIZE+
        MCU_FLASH_ADDR_START,24,(uint8_t *)"0123456789ABCDEF20200716");
    }
    //��2���̳�BIOS�Ĵ����ж�              
    void ** bios=MCU_FLASH_ADDR_START;   //��������ʼ��ַ  
    uint32_t user[MCU_SECTORSIZE/4];     //�������������
    //��ȡUSER�ж���������жϴ�������ַ����user����
    flash_read_physical((uint8_t *)user,GEC_USER_SECTOR_START*
    MCU_SECTORSIZE+MCU_FLASH_ADDR_START,MCU_SECTORSIZE);
    //�ж�USER�д��ڸ����ж��Ƿ�ΪBIOS��Ӧ�жϵ�ַ�����ǣ����������������޸�
    //ΪBIOS��Ӧ�жϵ�ַ
    if(user[BIOS_UART_UPDATE_IRQn] != (uint32_t)bios[BIOS_UART_UPDATE_IRQn])
    {
        //�ı�User�ж���������BIOS_UART�ж�����
        flash_erase(GEC_USER_SECTOR_START);
        //��USER�ж��������BIOS����д�봮�ڵ��жϴ�������ַ
        user[2]=(uint32_t)bios[2];     //��20200903��
        user[BIOS_UART_UPDATE_IRQn]=(uint32_t)bios[BIOS_UART_UPDATE_IRQn];
        flash_write_physical(GEC_USER_SECTOR_START*MCU_SECTORSIZE+
        MCU_FLASH_ADDR_START,MCU_SECTORSIZE,(uint8_t *)user);
    }
    
    //��3���ж�������ָ���ض���USER������ж�������
    SCB->VTOR = (uint32_t)(GEC_USER_SECTOR_START*MCU_SECTORSIZE+
    MCU_FLASH_ADDR_START);  
    //��4����component_fun��ֵ��SYSTEM_FUNCTION������
    component_fun=(void **)(MCU_FLASH_ADDR_START+
    GEC_COMPONENT_LST_START*MCU_SECTORSIZE); 
    //��5��printf��ʾ             
    printf("  ��User��ʾ����������User��main()ִ��...\r\n");
    
    #endif
}

