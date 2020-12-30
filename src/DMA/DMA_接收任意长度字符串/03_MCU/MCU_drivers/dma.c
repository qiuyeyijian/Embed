//=====================================================================
//�ļ����ƣ�dma.h
//���ܸ�Ҫ��dma�ײ���������ͷ�ļ�
//������λ����ͨ��ѧ ��181������
//��    ����  2020-12-31 V2.0
//����оƬ��KL36
//=====================================================================
#include "dma.h"

// DMA��ͨ���ж������
static const IRQn_Type dma_irq_table[] = {
    DMA0_IRQn,
    DMA1_IRQn,
    DMA2_IRQn,
    DMA3_IRQn,
};

//==========================================================================
//��������: dma_uart_init
//��������: ��
//����˵��:  chSend: DMA����ͨ��
//         chRecv: DMA����ͨ��
//         buff: DMA���ڴ��еĻ�����
//���ܸ�Ҫ: ��ʼ��UART��DMA����
//==========================================================================
void dma_uart_init(uint8_t chSend, uint8_t chRecv, uint32_t buff) {
  //ʹ��UART0��DMA����
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;  //��������0ʱ��
  UART0_C5 |= UART_C4_TDMAS_MASK;     //ʹ��UART0��TDMA
  UART0_C5 |= UART_C4_RDMAS_MASK;     //ʹ��UART0��RDMA
  // UART0_C2 |= UART_C2_TIE_MASK;       //ʹ��UART0�����ж�
  // UART0_C2 |= UART_C2_RIE_MASK;       //ʹ��UART0�����ж�
  // UART0_C2 &= ~UART_C2_TCIE_MASK;  //����UART0��������ж�

  //����DMA��DMAMUXģ��ʱ����
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  // ��DMA MUXģ��ʱ��
  SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;     // �� DMAģ��ʱ��
  // ��ʼ��DMAͨ�����üĴ�����Ĭ��DMAͨ����ֹ����ͨ����
  DMAMUX0_CHCFG(chSend) = 0x00;
  DMAMUX0_CHCFG(chRecv) = 0x00;
  // ����DMAͨ����������Դ
  DMAMUX0_CHCFG(chSend) |= DMAMUX_CHCFG_SOURCE(DMA_UART0_Tx);
  DMAMUX0_CHCFG(chRecv) |= DMAMUX_CHCFG_SOURCE(DMA_UART0_Rx);

  // ���DMA״̬�Ĵ���/�ֽ����Ĵ����еĴ����жϺ���ɱ�־
  dma_clear(chSend);
  dma_clear(chRecv);

  // ��ʼ��DMA���ƼĴ���
  // Ĭ�ϲ���������DMA���󣬼��������跢���DMA����
  DMA_DCR(chSend) = 0x00;
  DMA_DCR(chRecv) = 0x00;

  //����Դ��ַ��Ŀ���ַ��ģ����С�����ݿ���Լ�������ر�־
  DMA_DCR(chSend) |=
      (DMA_DCR_SSIZE(1)    //����Դ��ַ���ݳ���Ϊ8λ
       | DMA_DCR_DSIZE(1)  //Ŀ�ĵ�ַ���ݳ���Ϊ8λ
       | DMA_DCR_SMOD(0)  // ��ֹԴ��ַȡģ��ѭ��ģʽ����Ҫ����
       | DMA_DCR_DMOD(0)  // ��ֹĿ�ĵ�ַȡģ��ѭ��ģʽ��Ҫ����
       | DMA_DCR_D_REQ_MASK  // BCR��0ʱ�����������
       | DMA_DCR_SINC_MASK  // Դ��ַ�������ӣ�ÿ�δ�����Ŀ�ĵ�ַ����һ���ֽ�
       | DMA_DCR_CS_MASK   //ÿ�����󶼻����һ�ζ�д����
       | DMA_DCR_ERQ_MASK  //������������
       //| DMA_DCR_EINT_MASK  // ��������ж�ʹ��
       | DMA_DCR_EADREQ_MASK  //�����첽����

      );

  DMA_DCR(chRecv) |=
      (DMA_DCR_SSIZE(1)    //����Դ��ַ���ݳ���Ϊ8λ
       | DMA_DCR_DSIZE(1)  //Ŀ�ĵ�ַ���ݳ���Ϊ8λ
       | DMA_DCR_SMOD(0)  // ��ֹԴ��ַȡģ��ѭ��ģʽ����Ҫ����
       | DMA_DCR_DMOD(0)  // ��ֹĿ�ĵ�ַȡģ��ѭ��ģʽ��Ҫ����
       | DMA_DCR_D_REQ_MASK  // BCR��0ʱ�����������
       | DMA_DCR_DINC_MASK  // Դ��ַ�������ӣ�ÿ�δ�����Ŀ�ĵ�ַ����һ���ֽ�
       | DMA_DCR_CS_MASK   //ÿ�����󶼻����һ�ζ�д����
       | DMA_DCR_ERQ_MASK  //������������
       //| DMA_DCR_EINT_MASK  // ��������ж�ʹ��
       | DMA_DCR_EADREQ_MASK  //�����첽����

      );

  // ���÷���ͨ��Ŀ�ĵ�ַ, Ӧ����UART0�����ݼĴ�����ַ
  // ����ͨ����Դ��ַ��Ŀ�ĵ�ַ�����ڴ浽����UART0�����ݼĴ���
  DMA_SAR(chSend) = (uint32_t)buff;
  DMA_DAR(chSend) = (uint32_t)&UART0_D;
  // ����ͨ��Դ��ַ��Ŀ�ĵ�ַ��������UART0�����ݼĴ������ڴ�buff
  DMA_SAR(chRecv) = (uint32_t)&UART0_D;
  DMA_DAR(chRecv) = (uint32_t)buff;

  //����ͨ���ֽ��������Ĵ�������
  DMA_DSR_BCR(chSend) &= ~DMA_DSR_BCR_BCR_MASK;  // һ��Ҫ������
  DMA_DSR_BCR(chSend) = DMA_DSR_BCR_BCR(0);      // ��ʼ���Ȳ�Ҫ������
  //����ͨ���ֽ��������Ĵ�������
  // ÿ����һ���ֽڣ�����ֵ��һ��Ϊ0��ʱ���Զ�������������
  DMA_DSR_BCR(chRecv) &= ~DMA_DSR_BCR_BCR_MASK;  // һ��Ҫ������
  DMA_DSR_BCR(chRecv) = DMA_DSR_BCR_BCR(DMA_DSR_BCR_MAXLEN);

  // ʹ��DMAͨ��
  DMAMUX0_CHCFG(chSend) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMAMUX0_CHCFG(chRecv) |= (DMAMUX_CHCFG_ENBL_MASK);
}

//==========================================================================
//��������: dma_uart_recv
//��������: ��
//����˵��:  chRecv: DMA����ͨ��
//         dstAddress: Ŀ�ĵ�ַ��Դ��ַ�̶�ΪUART0�����ݼĴ�����ַ
//         len: ���յ����ݳ���
//���ܸ�Ҫ:  ʹ��һ��DMA���մ���
//==========================================================================
void dma_uart_recv(uint8_t chRecv, uint32_t dstAddress, uint32_t len) {
  //����Ŀ���ַ
  DMA_DAR(chRecv) = dstAddress;

  //���ж�������־
  dma_clear(chRecv);

  //��ʱ�رո�DMAͨ����Ĭ�ϴ���ģʽΪ����ģʽ
  DMA_DCR(chRecv) &= ~DMA_DCR_ERQ_MASK;
  DMAMUX0_CHCFG(chRecv) &= ~(DMAMUX_CHCFG_ENBL_MASK);

  //�ֽ��������Ĵ�������
  DMA_DSR_BCR(chRecv) &= ~DMA_DSR_BCR_BCR_MASK;  // һ��Ҫ������
  DMA_DSR_BCR(chRecv) = DMA_DSR_BCR_BCR(len);

  // ʹ�ܸ�DMAͨ��
  DMAMUX0_CHCFG(chRecv) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMA_DCR(chRecv) |= DMA_DCR_ERQ_MASK;
}

//==========================================================================
//��������: dma_uart_send
//��������: ��
//����˵��:  chSend: DMA����ͨ��
//         srcAddress: Դ��ַ��Ŀ�ĵ�ַ�̶�ΪUART0�����ݼĴ�����ַ
//         len: ���յ����ݳ���
//���ܸ�Ҫ:  ʹ��һ��DMA���ʹ���
//==========================================================================
void dma_uart_send(uint8_t chSend, uint32_t srcAddress, uint32_t len) {
  //�жϴ�����Ŀ�����Ƿ�Ϸ�
  if ((len & 0xFFF00000) > 0) {
    len = 0xFFFFF;
  }

  //���ж�������־
  dma_clear(chSend);

  //��ʱ�رո�DMAͨ���������ø�ͨ��������Դ��Ĭ�ϴ���ģʽΪ����ģʽ
  DMA_DCR(chSend) &= ~DMA_DCR_ERQ_MASK;
  DMAMUX0_CHCFG(chSend) &= ~(DMAMUX_CHCFG_ENBL_MASK);

  //����Ŀ�ĵ�ַ, Ӧ����UART0�����ݼĴ�����ַ
  DMA_DAR(chSend) = (uint32_t)&UART0_D;

  //����Դ��ַ
  DMA_SAR(chSend) = srcAddress;

  //�ֽ��������Ĵ�������
  DMA_DSR_BCR(chSend) &= ~DMA_DSR_BCR_BCR_MASK;  // һ��Ҫ������
  DMA_DSR_BCR(chSend) = DMA_DSR_BCR_BCR(len);

  //ʹ�ܸ�DMAͨ��
  DMAMUX0_CHCFG(chSend) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMA_DCR(chSend) |= DMA_DCR_ERQ_MASK;
}

//==========================================================================
//��������: dma_get_recv_count
//��������: ��
//����˵��:  chRecv: DMA����ͨ��
//���ܸ�Ҫ:  ��ȡDMA����ͨ���������ݸ���
//==========================================================================
uint32_t dma_get_recv_count(uint8_t chRecv) {
  // ���ֵ��ȥʣ���ֵ���ǽ��յ����ݸ���
  return (DMA_DSR_BCR_MAXLEN -
          (uint32_t)(DMA_DSR_BCR(chRecv) & DMA_DSR_BCR_BCR_MASK));
}

//==========================================================================
//��������: dma_enable_int
//��������: ��
//����˵��:  ch: DMAͨ��
//���ܸ�Ҫ:  ʹ��DMAͨ���ж�
//==========================================================================
void dma_enable_int(uint8_t ch) {
  DMA_DCR(ch) |= DMA_DCR_EINT_MASK;   // ����ͨ���ж�ʹ��
  NVIC_EnableIRQ(dma_irq_table[ch]);  //���жϿ�����IRQ�ж�
}

//======================================================================
//�������ƣ�dma_clear
//����˵����ch:
//�������أ���ͨ���ж�������־
//======================================================================
void dma_clear(uint8_t ch) {
  if ((DMA_DSR_BCR(ch) & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK) {
    DMA_DSR_BCR(ch) |= DMA_DSR_BCR_DONE_MASK;  //��DMA������ɱ�־, д1���
  }
}
