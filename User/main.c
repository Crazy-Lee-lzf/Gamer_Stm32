/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-07-19
 * @brief       LTDC LCD��RGB���� ʵ��
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/BTIM/btim.h"
#include "Task.h"


void Init(){
		sys_cache_enable();                         /* ��L1-Cache */
    HAL_Init();                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(432, 25, 2, 9);        /* ����ʱ��,216Mhz */
    delay_init(216);                            /* ��ʱ��ʼ�� */
    usart_init(115200);                         /* ���ڳ�ʼ�� */
    led_init();                                 /* ��ʼ��LED */
    mpu_memory_protection();                    /* ������ش洢���� */
    sdram_init();                               /* ��ʼ��SDRAM */
    lcd_init();                                 /* ��ʼ��LCD */
    key_init();                                 /* ��ʼ��KEY */
		btim_timx_int_init(1000 - 1, 1080 - 1);    /* 108 000 000 / 108 00 = 10KHz 10KHz�ļ���Ƶ�ʣ�����5K��Ϊ500ms */
		Task_Init();
}

extern TASK_COM_INFO myTask[MaxTaskNum];
extern uint8_t TaskNum;


int main(void)
{
		uint8_t i = 0;
		Init();
    while (1)
    {
			for(i = 0;i < TaskNum;i++){
				if(myTask[i].run == 1){		//�����־λΪ1 ˵������Ҫ��ʼ������
					myTask[i].Task();				//��������
					myTask[i].run = 0;			//���н���  ��־λ��0 �ȴ���һ��ִ��
				}
			}
    }
}


