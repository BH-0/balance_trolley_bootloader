#include "stmflash.h"
#include <string.h>
#include "w25qxx.h"

uint32_t app_size = 0;	//��Ҫ��¼����Ĵ�С

//��ȡָ����ַ����(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}

//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ  401CCU6���ֻ������5
//����ֵ:0~11,��addr���ڵ�����
u8 STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
//	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
//	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
//	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
//	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
//	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10;   
	return FLASH_SECTOR_5;	
}

//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F(ע�⣺���16�ֽڣ�����OTP���ݿ�����������д����)
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    
	HAL_FLASH_Unlock();             //����	
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			 if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //Ҫ����������
				FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//����������	
				}
				}else addrx+=4;
				FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //�ȴ��ϴβ������
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//д����
		 {
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(32λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}

/**
 * @bieaf ��������
 *	
 * @param sector_num    ������
 * @return 1
 */
int32_t sector_erase(uint32_t sector_num)
{
	int32_t rt=-1;
	FLASH_EraseInitTypeDef FlashEraseInit;
	u32 SectorError=0;
	/* ����FLASH*/
	HAL_FLASH_Unlock();             

	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
	FlashEraseInit.Sector = sector_num;   //Ҫ����������
	FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
	{
		return rt=-1;//����������	
	}
	else rt=0;

	FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������

	/* ����FLASH*/
	HAL_FLASH_Lock();
	
	return rt;
}

/* ��ȡ����ģʽ */
uint32_t read_start_mode(void)
{
	uint32_t mode = 0;
	uint8_t size_buf[4] = {0};	
	//flash_read((APPLICATION_2_ADDR + APPLICATION_2_SIZE - 4), &mode, 1);
	W25QXX_Read((uint8_t *)&mode, 64/8*1024*1024 - 4, 4);
	W25QXX_Read(size_buf, 64/8*1024*1024 - 8, 4);	//���������С
	app_size = 0;
	app_size |= (size_buf[0]&0xFF)<<24;
	app_size |= (size_buf[1]&0xFF)<<16;
	app_size |= (size_buf[2]&0xFF)<<8;
	app_size |= (size_buf[3]&0xFF)<<0;
	return mode;
}

/**
 * @bieaf ���г���ĸ���
 * @detail 1.����Ŀ�ĵ�ַ
 *         2.Դ��ַ�Ĵ��뿽����Ŀ�ĵ�ַ
 *         3.����Դ��ַ
 *
 * @param  ���˵�Ŀ�ĵ�ַ
 * @param  ���˵�Դ��ַ(SPI FLASH)
 * @return ���˵ĳ����С(byte)
 */
void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t size)
{
	uint8_t temp[256*4] = {0};
	uint32_t i;
	uint32_t rem = size%1024;
	
	/*1.����Ŀ�ĵ�ַ*/
	RTT_printf(0, "> start erase application 1 sector......\r\n");
	
	//����
	sector_erase(APPLICATION_1_SECTOR);
	
	RTT_printf(0, "> erase application 1 success......\r\n");
	
	/*2.��ʼ����*/	

	RTT_printf(0, "> start copy......\r\n");
	
	for(i = 0; i <size/1024; i++)
	{
		W25QXX_Read(temp, (src_addr + i*1024), 1024);
		STMFLASH_Write((dest_addr + i*1024), (uint32_t *)temp, 256);
		PCout(13) = ~PCout(13);
	}
	if(rem != 0)	//�������ֿ���
	{
		memset(temp, 0xFF, sizeof(temp));
		W25QXX_Read(temp, (src_addr + i*1024), rem);
		STMFLASH_Write((dest_addr + i*1024), (uint32_t *)temp, rem/4+1);
	}
	
	RTT_printf(0, "> copy finish......\r\n");
	
	/*3.����Դ��ַ*/
	
	RTT_printf(0, "> start erase application 2 sector......\r\n");
#if 1
	//����
	W25QXX_Erase_Block(src_addr + 0);	
	W25QXX_Erase_Block(src_addr + (64*1024));

	/* ���������־ */
	uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	W25QXX_Write(buf, 64/8*1024*1024 - 8, 8);
#endif 
	RTT_printf(0, "> erase application 2 success......\r\n");
	PCout(13) = 0;
}

//����Ӧ�ó���
void iap_execute_app (uint32_t app_addr)
{
	jump_func jump_to_app; 
	
	//RTT_printf(0, "* ( __IO uint32_t * ) app_addr  =%08X ,app_addr=%08X\r\n",* ( __IO uint32_t * ) app_addr,app_addr );
    
	//if ( ( ( * ( __IO uint32_t * ) app_addr ) & 0x2FFE0000 ) == 0x200006B0 )	//���ջ����ַ�Ƿ�Ϸ�.
	//{ 
		//RTT_printf(0, "stack is legal\r\n");
		
		jump_to_app = (jump_func) * ( __IO uint32_t *)(app_addr + 4);			//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		

		MSR_MSP( * ( __IO uint32_t * ) app_addr );								//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		
		for(int i = 0; i < 8; i++)
		{
			NVIC->ICER[i] = 0xFFFFFFFF; /* �ر��ж�*/
			NVIC->ICPR[i] = 0xFFFFFFFF; /* ����жϱ�־λ */
		}
        INTX_DISABLE();

		jump_to_app();															//��ת��APP.
	//}
	
	//RTT_printf(0, "stack is illegal\r\n");
}

//////////////////////////////////////////������///////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}
