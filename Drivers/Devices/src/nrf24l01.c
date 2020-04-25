#include "nrf24l01.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x52,0x01,0x31,0x45,0x20}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x52,0x01,0x31,0x45,0x20}; //���͵�ַ

/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
u8 SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi, u8 byte)
{
  u8 d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
  {
    d_read=0xFF;
  }
  return d_read; 
}

// ���24L01�Ƿ����
// ����ֵ:0���ɹ�;1��ʧ��
u8 NRF24L01_Check(void)
{
  u8 buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
  u8 i;
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //д��5���ֽڵĵ�ַ.
  NRF24L01_Read_Buf(TX_ADDR, buf, 5);                  //����д��ĵ�ַ
  for (i = 0; i < 5; i++)
    if (buf[i] != 0XA5)
      break;
  if (i != 5)
	{
		printf("NRF24L01����ģ���ʼ��ʧ�� error \r\n");
		return 1; //���24L01����
	}
	printf("NRF24L01����ģ���ʼ���ɹ� OK \r\n");
  return 0;   //��⵽24L01
}

// SPIд�Ĵ���
// reg:ָ���Ĵ�����ַ
// value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg, u8 value)
{
  u8 status;
  NRF24L01_CSN = 0;                 //ʹ��SPI����
  status =SPIx_ReadWriteByte(&hspi1, reg);   //���ͼĴ�����
  SPIx_ReadWriteByte(&hspi1, value);         //д��Ĵ�����ֵ
  NRF24L01_CSN = 1;                 //��ֹSPI����
  return (status);                  //����״ֵ̬
}

// ��ȡSPI�Ĵ���ֵ
// reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
  u8 reg_val;
  NRF24L01_CSN = 0;                   //ʹ��SPI����
  SPIx_ReadWriteByte(&hspi1, reg);   //���ͼĴ�����
  reg_val = SPIx_ReadWriteByte(&hspi1, 0XFF);//��ȡ�Ĵ�������
  NRF24L01_CSN = 1;                   //��ֹSPI����
  return (reg_val);                   //����״ֵ̬
}

// ��ָ��λ�ö���ָ�����ȵ�����
// reg:�Ĵ���(λ��)
// *pBuf:����ָ��
// len:���ݳ���
// ����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 //ʹ��SPI����
  status=SPIx_ReadWriteByte(&hspi1, reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
  {
    pBuf[u8_ctr]= SPIx_ReadWriteByte(&hspi1, 0XFF);//��������
  }
  NRF24L01_CSN = 1;                          //�ر�SPI����
  return status;                             //���ض�����״ֵ̬
}

// ��ָ��λ��дָ�����ȵ�����
// reg:�Ĵ���(λ��)
// *pBuf:����ָ��
// len:���ݳ���
// ����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 //ʹ��SPI����
  status = SPIx_ReadWriteByte(&hspi1, reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
  {
    SPIx_ReadWriteByte(&hspi1, *pBuf++); //д������	 
  }
  NRF24L01_CSN = 1;              //�ر�SPI����
  return status;                 //���ض�����״ֵ̬
}

// ����NRF24L01����һ������
// txbuf:�����������׵�ַ
// ����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
  u8 sta;
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); //д���ݵ�TX BUF  32���ֽ�
  NRF24L01_CE = 1;                                        //��������
  while (NRF24L01_IRQ != 0);                                          //�ȴ��������
  sta = NRF24L01_Read_Reg(STATUS);             //��ȡ״̬�Ĵ�����ֵ
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
  if (sta & MAX_TX)                            //�ﵽ����ط�����
  {
    NRF24L01_Write_Reg(FLUSH_TX, 0xff); //���TX FIFO�Ĵ���
    return MAX_TX;
  }
  if (sta & TX_OK) //�������
  {
    return TX_OK;
  }
  return 0xff; //����ԭ����ʧ��
}

// ����NRF24L01����һ������
// rxbuf:�����������׵�ַ
// ����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
  u8 sta;
  sta = NRF24L01_Read_Reg(STATUS);             //��ȡ״̬�Ĵ�����ֵ
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
  if (sta & RX_OK)                             //���յ�����
  {
    NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); //��ȡ����
    NRF24L01_Write_Reg(FLUSH_RX, 0xff);                    //���RX FIFO�Ĵ���
    return 0;
  }
  return 1; //û�յ��κ�����
}

// �ú�����ʼ��NRF24L01��RXģʽ
// ����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
// ��CE��ߺ�,������RXģʽ,�����Խ���������
void RX_Mode(void)
{
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);              //ʹ��ͨ��0���Զ�Ӧ��
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);          //ʹ��ͨ��0�Ľ��յ�ַ
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);                //����RFͨ��Ϊ40
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ��
  // NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);           //����TX�������,0db����,2Mbps,���������濪��
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0B);           // 0db����,250kbps
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);             //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
  NRF24L01_CE = 1;                                          //CEΪ��,�������ģʽ
}

// �ú�����ʼ��NRF24L01��TXģʽ
// ����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
// PWR_UP,CRCʹ��
// ��CE��ߺ�,������RXģʽ,�����Խ���������
// CEΪ�ߴ���10us,����������.
void TX_Mode(void)
{
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);    //дTX�ڵ��ַ
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);      //ʹ��ͨ��0���Զ�Ӧ��
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  //ʹ��ͨ��0�Ľ��յ�ַ
  NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a); //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);        //����RFͨ��Ϊ40
  // NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);   //����TX�������,0db����,2Mbps,���������濪��
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0B);           // 0db����,250kbps
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);     //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
  NRF24L01_CE = 1;                                  //CEΪ��,10us����������
}

// �ú���NRF24L01����͹���ģʽ
void NRF_LowPower_Mode(void)
{
	NRF24L01_CE = 0;
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x00);		//���ù���ģʽ:����ģʽ
}

// NRF24L01��ʼ��
void NRF24L01_Init(void)
{
  NRF24L01_CE = 0;  //ʹ��24L01
  NRF24L01_CSN = 1; //SPIƬѡȡ��
  NRF24L01_Check();   // ���NRF24L01 �����Ƿ�����
  // ң�� ��ʼ��Ϊ����״̬
  if(NRF24L01_MODEL == 0)
  {
    RX_Mode();
    printf("ң�� -> Ԥ�ƽ�NRF24L01��ʼ��Ϊ����״̬\r\n");
  }
  // ������ ��ʼ��Ϊ����״̬
  else if(NRF24L01_MODEL == 1)
  {
    TX_Mode();
    printf("������ -> Ԥ�ƽ�NRF24L01��ʼ��Ϊ����״̬\r\n");
  }
}
