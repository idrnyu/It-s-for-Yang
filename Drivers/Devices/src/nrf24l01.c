#include "nrf24l01.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x52,0x01,0x31,0x45,0x20}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x52,0x01,0x31,0x45,0x20}; //发送地址

/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
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

// 检测24L01是否存在
// 返回值:0，成功;1，失败
u8 NRF24L01_Check(void)
{
  u8 buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
  u8 i;
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //写入5个字节的地址.
  NRF24L01_Read_Buf(TX_ADDR, buf, 5);                  //读出写入的地址
  for (i = 0; i < 5; i++)
    if (buf[i] != 0XA5)
      break;
  if (i != 5)
	{
		printf("NRF24L01无线模块初始化失败 error \r\n");
		return 1; //检测24L01错误
	}
	printf("NRF24L01无线模块初始化成功 OK \r\n");
  return 0;   //检测到24L01
}

// SPI写寄存器
// reg:指定寄存器地址
// value:写入的值
u8 NRF24L01_Write_Reg(u8 reg, u8 value)
{
  u8 status;
  NRF24L01_CSN = 0;                 //使能SPI传输
  status =SPIx_ReadWriteByte(&hspi1, reg);   //发送寄存器号
  SPIx_ReadWriteByte(&hspi1, value);         //写入寄存器的值
  NRF24L01_CSN = 1;                 //禁止SPI传输
  return (status);                  //返回状态值
}

// 读取SPI寄存器值
// reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
  u8 reg_val;
  NRF24L01_CSN = 0;                   //使能SPI传输
  SPIx_ReadWriteByte(&hspi1, reg);   //发送寄存器号
  reg_val = SPIx_ReadWriteByte(&hspi1, 0XFF);//读取寄存器内容
  NRF24L01_CSN = 1;                   //禁止SPI传输
  return (reg_val);                   //返回状态值
}

// 在指定位置读出指定长度的数据
// reg:寄存器(位置)
// *pBuf:数据指针
// len:数据长度
// 返回值,此次读到的状态寄存器值
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 //使能SPI传输
  status=SPIx_ReadWriteByte(&hspi1, reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
  {
    pBuf[u8_ctr]= SPIx_ReadWriteByte(&hspi1, 0XFF);//读出数据
  }
  NRF24L01_CSN = 1;                          //关闭SPI传输
  return status;                             //返回读到的状态值
}

// 在指定位置写指定长度的数据
// reg:寄存器(位置)
// *pBuf:数据指针
// len:数据长度
// 返回值,此次读到的状态寄存器值
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 //使能SPI传输
  status = SPIx_ReadWriteByte(&hspi1, reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
  {
    SPIx_ReadWriteByte(&hspi1, *pBuf++); //写入数据	 
  }
  NRF24L01_CSN = 1;              //关闭SPI传输
  return status;                 //返回读到的状态值
}

// 启动NRF24L01发送一次数据
// txbuf:待发送数据首地址
// 返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf)
{
  u8 sta;
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); //写数据到TX BUF  32个字节
  NRF24L01_CE = 1;                                        //启动发送
  while (NRF24L01_IRQ != 0);                                          //等待发送完成
  sta = NRF24L01_Read_Reg(STATUS);             //读取状态寄存器的值
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
  if (sta & MAX_TX)                            //达到最大重发次数
  {
    NRF24L01_Write_Reg(FLUSH_TX, 0xff); //清除TX FIFO寄存器
    return MAX_TX;
  }
  if (sta & TX_OK) //发送完成
  {
    return TX_OK;
  }
  return 0xff; //其他原因发送失败
}

// 启动NRF24L01接收一次数据
// rxbuf:待接收数据首地址
// 返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
  u8 sta;
  sta = NRF24L01_Read_Reg(STATUS);             //读取状态寄存器的值
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
  if (sta & RX_OK)                             //接收到数据
  {
    NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); //读取数据
    NRF24L01_Write_Reg(FLUSH_RX, 0xff);                    //清除RX FIFO寄存器
    return 0;
  }
  return 1; //没收到任何数据
}

// 该函数初始化NRF24L01到RX模式
// 设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
// 当CE变高后,即进入RX模式,并可以接收数据了
void RX_Mode(void)
{
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //写RX节点地址
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);              //使能通道0的自动应答
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);          //使能通道0的接收地址
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);                //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //选择通道0的有效数据宽度
  // NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);           //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0B);           // 0db增益,250kbps
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);             //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
  NRF24L01_CE = 1;                                          //CE为高,进入接收模式
}

// 该函数初始化NRF24L01到TX模式
// 设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
// PWR_UP,CRC使能
// 当CE变高后,即进入RX模式,并可以接收数据了
// CE为高大于10us,则启动发送.
void TX_Mode(void)
{
  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);    //写TX节点地址
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);      //使能通道0的自动应答
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  //使能通道0的接收地址
  NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a); //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);        //设置RF通道为40
  // NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);   //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0B);           // 0db增益,250kbps
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);     //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
  NRF24L01_CE = 1;                                  //CE为高,10us后启动发送
}

// 该函数NRF24L01进入低功耗模式
void NRF_LowPower_Mode(void)
{
	NRF24L01_CE = 0;
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x00);		//配置工作模式:掉电模式
}

// NRF24L01初始化
void NRF24L01_Init(void)
{
  NRF24L01_CE = 0;  //使能24L01
  NRF24L01_CSN = 1; //SPI片选取消
  NRF24L01_Check();   // 检测NRF24L01 工作是否正常
  // 遥控 初始化为接收状态
  if(NRF24L01_MODEL == 0)
  {
    RX_Mode();
    printf("遥控 -> 预计将NRF24L01初始化为接收状态\r\n");
  }
  // 控制器 初始化为发送状态
  else if(NRF24L01_MODEL == 1)
  {
    TX_Mode();
    printf("控制器 -> 预计将NRF24L01初始化为发送状态\r\n");
  }
}
