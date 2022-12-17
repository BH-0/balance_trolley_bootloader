#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "sys.h"

/*=====�û�����(�����Լ��ķ�����������)=====*/
#define BootLoader_Size 		0x4000U			///< BootLoader�Ĵ�С 16K
#define Application_Size		0x20000U		///< Ӧ�ó���Ĵ�С 128K

#define APPLICATION_1_ADDR		0x08020000U		///< Ӧ�ó���1���׵�ַ������5�׵�ַ
#define APPLICATION_2_ADDR		0x08040000U		///< Ӧ�ó���2���׵�ַ������6�׵�ַ

#define APPLICATION_1_SECTOR	FLASH_SECTOR_5	///< Ӧ�ó���1������
#define APPLICATION_2_SECTOR	FLASH_SECTOR_6	///< Ӧ�ó���2������


#define APPLICATION_1_SIZE		0x20000U		///< Ӧ�ó���1�Ĵ�С
#define APPLICATION_2_SIZE		0x20000U		///< Ӧ�ó���2�Ĵ�С

/*==========================================*/


/* �����Ĳ��� */
#define STARTUP_NORMAL 0xFFFFFFFF	///< ��������
#define STARTUP_UPDATE 0xAAAAAAAA	///< ����������
#define STARTUP_RESET  0x5555AAAA	///< �ָ�����,Ŀǰûʹ��

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes 

/* ������ת���� */
typedef void (*jump_func)(void);

extern uint32_t app_size;	//��Ҫ��¼����Ĵ�С
 
u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

/* ��ȡ����ģʽ */
uint32_t read_start_mode(void);

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
void move_code(uint32_t dest_addr, uint32_t src_addr,uint32_t size);

//����Ӧ�ó���
void iap_execute_app (uint32_t app_addr);

//����д��
void Test_Write(u32 WriteAddr,u32 WriteData);	
#endif
