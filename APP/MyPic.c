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

/**
 * @brief       得到path路径下,目标文件的总个数
 * @param       path : 路径
 * @retval      总有效文件数
 */
uint16_t pic_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;                                                 /* 临时目录 */
    FILINFO *tfileinfo;                                       /* 临时文件信息 */
    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); /* 申请内存 */
    res = f_opendir(&tdir, (const TCHAR *)path);              /* 打开目录 */

    if (res == FR_OK && tfileinfo)
    {
        while (1)                               /* 查询总的有效文件数 */
        {
            res = f_readdir(&tdir, tfileinfo);  /* 读取目录下的一个文件 */

            if (res != FR_OK || tfileinfo->fname[0] == 0)
            {
                break;                          /* 错误了/到末尾了,退出 */
            }

            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0XF0) == 0X50)           /* 取高四位,看看是不是图片文件 */
            {
                rval++;                         /* 有效文件数增加1 */
            }
        }
    }

    myfree(SRAMIN, tfileinfo);                  /* 释放内存 */
    return rval;
}

uint8_t res;
DIR picdir;             /* 图片目录 */
FILINFO *picfileinfo;   /* 文件信息 */
char *pname;            /* 带路径的文件名 */
uint16_t totpicnum;     /* 图片文件总数 */
uint16_t curindex;      /* 图片当前索引 */
uint8_t key;            /* 键值 */
uint8_t pause = 0;      /* 暂停标记 */
uint8_t t;
uint16_t temp;
uint32_t *picoffsettbl; /* 图片文件offset索引表 */

void MyPic_Init(){
    my_mem_init(SRAMIN);                        /* 初始化内部内存池 */
    my_mem_init(SRAMEX);                        /* 初始化外部内存池 */
    my_mem_init(SRAMTCM);                       /* 初始化DTCM内存池 */

    exfuns_init();                              /* 为fatfs相关变量申请内存 */

    f_mount(fs[0], "0:", 1);                    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);                    /* 挂载SPI FLASH */
    f_mount(fs[2], "2:", 1);                    /* 挂载NAND FLASH */

    while (fonts_init())                        /* 检查字库 */
    {
        lcd_clear(WHITE);                       /* 清屏 */
        lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);

        while (sd_init())                       /* 检测SD卡 */
        {
            lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
            delay_ms(200);
            lcd_fill(30, 50, 200 + 30, 50 + 16, WHITE);
            delay_ms(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "SD Card OK", RED);
        lcd_show_string(30, 70, 200, 16, 16, "Font Updating...", RED);
        key = fonts_update_font(20, 90, 16, (uint8_t *)"0:", RED);  /* 更新字库 */

        while (key)       /* 更新失败 */
        {
            lcd_show_string(30, 90, 200, 16, 16, "Font Update Failed!", RED);
            delay_ms(200);
            lcd_fill(20, 90, 200 + 20, 90 + 16, WHITE);
            delay_ms(200);
        }

        lcd_show_string(30, 90, 200, 16, 16, "Font Update Success!   ", RED);
        delay_ms(1500);
        lcd_clear(WHITE);   /* 清屏 */
    } 
		
		totpicnum = pic_get_tnum("0:/PICTURE");     /* 得到总有效文件数 */
    
    picfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); /* 申请内存 */
    pname = mymalloc(SRAMIN, FF_MAX_LFN * 2 + 1);               /* 为带路径的文件名分配内存 */
    picoffsettbl = mymalloc(SRAMIN, 4 * totpicnum);             /* 申请4*totpicnum个字节的内存,用于存放图片索引 */
}

void StartPic(void){
    piclib_init();
}

void ShowPic(char* name, uint16_t sx, uint16_t sy, uint16_t w, uint16_t h){
    piclib_ai_load_picfile(name, sx, sy, w, h, 1); /* 显示图片 */
}

void StopPic(void){
    myfree(SRAMIN, picfileinfo);  /* 释放内存 */
    myfree(SRAMIN, pname);        /* 释放内存 */
    myfree(SRAMIN, picoffsettbl); /* 释放内存 */
}