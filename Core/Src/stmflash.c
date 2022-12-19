#include "stmflash.h"
#include <string.h>
#include "w25qxx.h"

uint32_t app_size = 0;	//需要烧录程序的大小

//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}

//获取某个地址所在的flash扇区
//addr:flash地址  401CCU6最大只有扇区5
//返回值:0~11,即addr所在的扇区
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

//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    
	HAL_FLASH_Unlock();             //解锁	
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			 if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
				FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//发生错误了	
				}
				}else addrx+=4;
				FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//写数据
		 {
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}

/**
 * @bieaf 扇区擦除
 *	
 * @param sector_num    扇区号
 * @return 1
 */
int32_t sector_erase(uint32_t sector_num)
{
	int32_t rt=-1;
	FLASH_EraseInitTypeDef FlashEraseInit;
	u32 SectorError=0;
	/* 解锁FLASH*/
	HAL_FLASH_Unlock();             

	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
	FlashEraseInit.Sector = sector_num;   //要擦除的扇区
	FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
	{
		return rt=-1;//发生错误了	
	}
	else rt=0;

	FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成

	/* 锁定FLASH*/
	HAL_FLASH_Lock();
	
	return rt;
}

/* 读取启动模式 */
uint32_t read_start_mode(void)
{
	uint32_t mode = 0;
	uint8_t size_buf[4] = {0};	
	//flash_read((APPLICATION_2_ADDR + APPLICATION_2_SIZE - 4), &mode, 1);
	W25QXX_Read((uint8_t *)&mode, 64/8*1024*1024 - 4, 4);
	W25QXX_Read(size_buf, 64/8*1024*1024 - 8, 4);	//读出程序大小
	app_size = 0;
	app_size |= (size_buf[0]&0xFF)<<24;
	app_size |= (size_buf[1]&0xFF)<<16;
	app_size |= (size_buf[2]&0xFF)<<8;
	app_size |= (size_buf[3]&0xFF)<<0;
	return mode;
}

/**
 * @bieaf 进行程序的覆盖
 * @detail 1.擦除目的地址
 *         2.源地址的代码拷贝到目的地址
 *         3.擦除源地址
 *
 * @param  搬运的目的地址
 * @param  搬运的源地址(SPI FLASH)
 * @return 搬运的程序大小(byte)
 */
void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t size)
{
	uint8_t temp[256*4] = {0};
	uint32_t i;
	uint32_t rem = size%1024;
	
	/*1.擦除目的地址*/
	RTT_printf(0, "> start erase application 1 sector......\r\n");
	
	//擦除
	sector_erase(APPLICATION_1_SECTOR);
	
	RTT_printf(0, "> erase application 1 success......\r\n");
	
	/*2.开始拷贝*/	

	RTT_printf(0, "> start copy......\r\n");
	
	for(i = 0; i <size/1024; i++)
	{
		W25QXX_Read(temp, (src_addr + i*1024), 1024);
		STMFLASH_Write((dest_addr + i*1024), (uint32_t *)temp, 256);
		PCout(13) = ~PCout(13);
	}
	if(rem != 0)	//余数部分拷贝
	{
		memset(temp, 0xFF, sizeof(temp));
		W25QXX_Read(temp, (src_addr + i*1024), rem);
		STMFLASH_Write((dest_addr + i*1024), (uint32_t *)temp, rem/4+1);
	}
	
	RTT_printf(0, "> copy finish......\r\n");
	
	/*3.擦除源地址*/
	
	RTT_printf(0, "> start erase application 2 sector......\r\n");
#if 1
	//擦除
	W25QXX_Erase_Block(src_addr + 0);	
	W25QXX_Erase_Block(src_addr + (64*1024));

	/* 清除升级标志 */
	uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	W25QXX_Write(buf, 64/8*1024*1024 - 8, 8);
#endif 
	RTT_printf(0, "> erase application 2 success......\r\n");
	PCout(13) = 0;
}

//启动应用程序
void iap_execute_app (uint32_t app_addr)
{
	jump_func jump_to_app; 
	
	//RTT_printf(0, "* ( __IO uint32_t * ) app_addr  =%08X ,app_addr=%08X\r\n",* ( __IO uint32_t * ) app_addr,app_addr );
    
	//if ( ( ( * ( __IO uint32_t * ) app_addr ) & 0x2FFE0000 ) == 0x200006B0 )	//检查栈顶地址是否合法.
	//{ 
		//RTT_printf(0, "stack is legal\r\n");
		
		jump_to_app = (jump_func) * ( __IO uint32_t *)(app_addr + 4);			//用户代码区第二个字为程序开始地址(复位地址)		

		MSR_MSP( * ( __IO uint32_t * ) app_addr );								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		
		for(int i = 0; i < 8; i++)
		{
			NVIC->ICER[i] = 0xFFFFFFFF; /* 关闭中断*/
			NVIC->ICPR[i] = 0xFFFFFFFF; /* 清除中断标志位 */
		}
        INTX_DISABLE();

		jump_to_app();															//跳转到APP.
	//}
	
	//RTT_printf(0, "stack is illegal\r\n");
}

//////////////////////////////////////////测试用///////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}
