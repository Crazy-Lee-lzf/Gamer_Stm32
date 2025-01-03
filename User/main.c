/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-07-19
 * @brief       图片显示 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F767开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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
    sys_cache_enable();                         /* 打开L1-Cache */
    HAL_Init();                                 /* 初始化HAL库 */
    mpu_memory_protection();                    /* 保护相关存储区域 */
    sys_stm32_clock_init(432, 25, 2, 9);        /* 设置时钟,216Mhz */
    delay_init(216);                            /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化 */
    usmart_dev.init(108);                       /* USMART初始化 */
    led_init();                                 /* 初始化LED */
    sdram_init();                               /* 初始化SDRAM */
    lcd_init();                                 /* 初始化LCD */
    key_init();                                 /* 初始化按键 */
    norflash_init();                            /* 初始化W25Q256 */
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
                if(myTask[i].run == 1){		//如果标志位为1 说明任务要开始运行了
                        myTask[i].Task();				//运行任务
                        myTask[i].run = 0;			//运行结束  标志位置0 等待下一次执行
                }
            }
    }
}


