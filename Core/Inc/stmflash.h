#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "sys.h"

/*=====用户配置(根据自己的分区进行配置)=====*/
#define BootLoader_Size 		0x4000U			///< BootLoader的大小 16K
#define Application_Size		0x20000U		///< 应用程序的大小 128K

#define APPLICATION_1_ADDR		0x08020000U		///< 应用程序1的首地址，扇区5首地址
#define APPLICATION_2_ADDR		0x08040000U		///< 应用程序2的首地址，扇区6首地址

#define APPLICATION_1_SECTOR	FLASH_SECTOR_5	///< 应用程序1的扇区
#define APPLICATION_2_SECTOR	FLASH_SECTOR_6	///< 应用程序2的扇区


#define APPLICATION_1_SIZE		0x20000U		///< 应用程序1的大小
#define APPLICATION_2_SIZE		0x20000U		///< 应用程序2的大小

/*==========================================*/


/* 启动的步骤 */
#define STARTUP_NORMAL 0xFFFFFFFF	///< 正常启动
#define STARTUP_UPDATE 0xAAAAAAAA	///< 升级再启动
#define STARTUP_RESET  0x5555AAAA	///< 恢复出厂,目前没使用

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes 

/* 程序跳转函数 */
typedef void (*jump_func)(void);

extern uint32_t app_size;	//需要烧录程序的大小
 
u32 STMFLASH_ReadWord(u32 faddr);		  	//读出字  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//从指定地址开始读出指定长度的数据

/* 读取启动模式 */
uint32_t read_start_mode(void);

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
void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t size);

//启动应用程序
void iap_execute_app (uint32_t app_addr);

//测试写入
void Test_Write(u32 WriteAddr,u32 WriteData);	
#endif
