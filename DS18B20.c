#include <msp430.h>
//�Զ������ݽṹ������ʹ��
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long
//DS18B20���ƽţ����ſ���
#define DQ_IN	        P2DIR &= ~BIT4		  //�������룬DS18B20�ӵ�Ƭ��P53��
#define DQ_OUT	        P2DIR |= BIT4		  //�������
#define DQ_CLR	        P2OUT &= ~BIT4	          //�õ͵�ƽ
#define DQ_SET	        P2OUT |= BIT4	          //�øߵ�ƽ
#define DQ_R	        P2IN & BIT4		  //����ƽ
uint    temp_value;
uint    temp;
uchar   A1,A2,A3;             		  //����ı���,��ʾ���ݴ���
uchar   Temp_Value[6];                     //��ʾ�õ��¶����ݣ������÷�������׼������

/*******************************************
�������ƣ�delay_us
��    �ܣ�ʵ��N��΢�����ʱ
��    ����n--��ʱ����
����ֵ  ����
˵��    ����ʱ��A�ļ���ʱ����1MHz��CPU��Ƶ8MHz
          ����ͨ����ʱ����ʱ�ܹ��õ���Ϊ��ȷ��
          us����ʱ
********************************************/
void delay_us(uint n)
{
  uint i;
  for(i=0;i<n;i++) _NOP();
  
}

//************************************************************************* 
//			DS18B20��ʼ��
//*************************************************************************
unsigned char DS18B20_Reset(void)                //��ʼ���͸�λ
{
  unsigned char i;
  DQ_OUT;
  DQ_CLR;
  delay_us(500);				//��ʱ500uS(480-960)
  DQ_SET;
  DQ_IN;
  delay_us(80);				        //��ʱ80uS
  i = DQ_R;
  delay_us(500);				//��ʱ500uS(����>480uS)
	
  if (i) 
  {
    return 0x00;
  }
  else 
  {
    return 0x01;
  }
}

//************************************************************************* 
//			DS18B20��һ���ֽں���
//************************************************************************* 
   
unsigned char ds1820_read_byte(void) 
{
  unsigned char i;
  unsigned char value = 0;
  for (i = 8; i != 0; i--) 
  {
    value >>= 1;
    DQ_OUT;
    DQ_CLR;
    delay_us(4);			        //*��ʱ4uS	
    DQ_SET;
    DQ_IN;
    delay_us(10);			        //*��ʱ10uS	
    if (DQ_R) 
    {
      value|=0x80;		
    }
    delay_us(60);			        //*��ʱ60uS	
  }
  return(value);
}

//************************************************************************* 
//			��18B20дһ���ֽں���
//*************************************************************************  

/*DS18B20�ֽ�д�뺯��*/
void ds1820_write_byte(unsigned char value) 
{
  unsigned char i;
  for (i = 8; i != 0; i--) 
  {
    DQ_OUT;
    DQ_CLR;
    delay_us(4);			  //��ʱ4uS
    if (value & 0x01) 
    {
      DQ_SET;		
    }
    delay_us(80);			  //��ʱ80uS
    DQ_SET;			          //λ����
    value >>= 1;	
  }
}
//*************************************************************************
//				�����¶�ת������
//************************************************************************* 

/*����ds1820ת��*/
void ds1820_start(void) 
{
  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //���Ե�ַ
  ds1820_write_byte(0x44);	          //����ת��
}

//*************************************************************************
//				DS8B20��ȡ�¶���Ϣ
//************************************************************************* 

unsigned int ds1820_read_temp(void) 
{
  unsigned int i;
  unsigned char buf[9];

  DS18B20_Reset();
  ds1820_write_byte(0xCC);	          //���Ե�ַ
  ds1820_write_byte(0xBE);	          //��ȡ�¶�
  for (i = 0; i < 9; i++) 
  {
    buf[i] = ds1820_read_byte();	
  }
  i = buf[1];
  i <<= 8;
  i |= buf[0];
  temp_value=i;
  temp_value=(uint)(temp_value*0.625);    //���ǳ���0.0625��ԭ����Ϊ�˰�С�����һλ����Ҳת��Ϊ������ʾ������
		                          //�����¶ȱ���Ϊ27.5�ȣ�Ϊ���ں��������ݴ�������еõ�BCD�룬�����ȷŴ�275
                                          //Ȼ������ʾ��ʱ��ȷ��С�����λ�ü��ɣ�������ʾ��27.5����
  return i;
}

//*************************************************************************
//		�¶����ݴ�����
//*************************************************************************
 void data_do(uint temp_d)
 {
   uint A2t;
   A1=temp_d/100;                         //�ֳ��٣�ʮ���͸�λ
   A2t=temp_d%100;
   A2=A2t/10;
   A3=A2t%10;
   Temp_Value[0]=A1+0x30;
   Temp_Value[1]=A2+0x30;
   Temp_Value[2]='.';
   Temp_Value[3]=A3+0x30;
   Temp_Value[4]='C';
   Temp_Value[5]=' ';
}
