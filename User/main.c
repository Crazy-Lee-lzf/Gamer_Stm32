/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-07-19
 * @brief       ͼƬ��ʾ ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ F767������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "math.h"
#include "string.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"
#include "./PICTURE/piclib.h"
#include "./TEXT/text.h"
#include "./BSP/MPU/mpu.h"
#include "./TEXT/fonts.h"
#include "./BSP/BTIM/btim.h"
#include "Task.h"
#include "MyPic.h"
#include "Snake.h"


void Init(){
    sys_cache_enable();                         /* ��L1-Cache */
    HAL_Init();                                 /* ��ʼ��HAL�� */
    mpu_memory_protection();                    /* ������ش洢���� */
    sys_stm32_clock_init(432, 25, 2, 9);        /* ����ʱ��,216Mhz */
    delay_init(216);                            /* ��ʱ��ʼ�� */
    usart_init(115200);                         /* ���ڳ�ʼ�� */
    usmart_dev.init(108);                       /* USMART��ʼ�� */
    led_init();                                 /* ��ʼ��LED */
    sdram_init();                               /* ��ʼ��SDRAM */
    lcd_init();                                 /* ��ʼ��LCD */
    key_init();                                 /* ��ʼ������ */
    norflash_init();                            /* ��ʼ��W25Q256 */
    btim_timx_int_init(1000 - 1, 1080 - 1);
    Task_Init();
    MyPic_Init();
    
}

extern TASK_COM_INFO myTask[MaxTaskNum];
extern uint8_t TaskNum;

int main(void)
{
    uint8_t i = 0;
    Init();
    while (1){
        for(i = 0;i < TaskNum;i++){
                if(myTask[i].run == 1){		//�����־λΪ1 ˵������Ҫ��ʼ������
                        myTask[i].Task();				//��������
                        myTask[i].run = 0;			//���н���  ��־λ��0 �ȴ���һ��ִ��
                }
            }
    }
}


