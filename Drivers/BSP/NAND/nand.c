/**
 ****************************************************************************************************
 * @file        nand.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-7-19
 * @brief       NAND FLASH 驱动代码
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
 * 修改说明
 * V1.0 20220719
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include <stdio.h>
#include "./BSP/NAND/nand.h"
#include "./SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"


NAND_HandleTypeDef g_nand_handle;    /* NAND FLASH句柄 */
nand_attriute nand_dev;              /* nand重要参数结构体 */

/**
 * @brief       初始化NAND FLASH
 * @param       无
 * @retval      0,成功;1,错误;
 */
uint8_t nand_init(void)
{
    FMC_NAND_PCC_TimingTypeDef comspacetiming, attspacetiming;

    nand_mpu_config();
    g_nand_handle.Instance = FMC_Bank3;
    g_nand_handle.Init.NandBank = FMC_NAND_BANK3;                          /* NAND挂在BANK3上 */
    g_nand_handle.Init.Waitfeature = FMC_NAND_PCC_WAIT_FEATURE_DISABLE;    /* 关闭等待特性 */
    g_nand_handle.Init.MemoryDataWidth = FMC_NAND_PCC_MEM_BUS_WIDTH_8;     /* 8位数据宽度 */
    g_nand_handle.Init.EccComputation = FMC_NAND_ECC_DISABLE;              /* 不使用ECC */
    g_nand_handle.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_512BYTE;       /* ECC页大小为0.5k */
    g_nand_handle.Init.TCLRSetupTime = 7;                                  /* 设置TCLR(tCLR=CLE到RE的延时)=(TCLR+TSET+2)*THCLK,THCLK=1/216M=4.6ns */
    g_nand_handle.Init.TARSetupTime = 7;                                   /* 设置TAR(tAR=ALE到RE的延时)=(TAR+TSET+1)*THCLK,THCLK=1/216M=4.6n */
   
    comspacetiming.SetupTime = 3;         /* 建立时间 */
    comspacetiming.WaitSetupTime = 4;     /* 等待时间 */
    comspacetiming.HoldSetupTime = 3;     /* 保持时间 */
    comspacetiming.HiZSetupTime = 3;      /* 高阻态时间 */
    
    attspacetiming.SetupTime = 3;         /* 建立时间 */
    attspacetiming.WaitSetupTime = 4;     /* 等待时间 */
    attspacetiming.HoldSetupTime = 3;     /* 保持时间 */
    attspacetiming.HiZSetupTime = 3;      /* 高阻态时间 */
    
    HAL_NAND_Init(&g_nand_handle, &comspacetiming, &attspacetiming); 
    nand_reset();                         /* 复位NAND */
    delay_ms(100);
    nand_dev.id = nand_readid();          /* 读取ID */
    printf("NAND ID:%#x\r\n", nand_dev.id);
    nand_modeset(4);                      /* 设置为MODE4,高速模式 */

    if (nand_dev.id == MT29F16G08ABABA)     /* NAND为MT29F16G08ABABA */
    {
        nand_dev.page_totalsize = 4320;     /* nand一个page的总大小（包括spare区） */
        nand_dev.page_mainsize = 4096;      /* nand一个page的有效数据区大小 */
        nand_dev.page_sparesize = 224;      /* nand一个page的spare区大小 */
        nand_dev.block_pagenum = 128;       /* nand一个block所包含的page数目 */
        nand_dev.plane_blocknum = 2048;     /* nand一个plane所包含的block数目 */
        nand_dev.block_totalnum = 4096;     /* nand的总block数目 */
    }
    else if (nand_dev.id == MT29F4G08ABADA) /* NAND为MT29F4G08ABADA */
    {
        nand_dev.page_totalsize = 2112;     /* nand一个page的总大小（包括spare区） */
        nand_dev.page_mainsize = 2048;      /* nand一个page的有效数据区大小 */
        nand_dev.page_sparesize = 64;       /* nand一个page的spare区大小 */
        nand_dev.block_pagenum = 64;        /* nand一个block所包含的page数目 */
        nand_dev.plane_blocknum = 2048;     /* nand一个plane所包含的block数目 */
        nand_dev.block_totalnum = 4096;     /* nand的总block数目 */
    }
    else return 1;                          /* 错误，返回 */

    return 0;
}

/**
 * @brief       配置MPU的region
 * @param       无
 * @retval      无
 */
void nand_mpu_config(void)
{
    MPU_Region_InitTypeDef mpu_initure;

    HAL_MPU_Disable();      /* 配置MPU之前先关闭MPU,配置完成以后在使能MPU */

    /* 配置RAM为region1，大小为256MB，此区域可读写 */
    mpu_initure.Enable = MPU_REGION_ENABLE;       /* 使能region */
    mpu_initure.Number = NAND_REGION_NUMBER;      /* 设置region，NAND使用的region0 */
    mpu_initure.BaseAddress = NAND_ADDRESS_START; /* region基地址 */
    mpu_initure.Size = NAND_REGION_SIZE;          /* region大小 */
    mpu_initure.SubRegionDisable = 0X00;
    mpu_initure.TypeExtField = MPU_TEX_LEVEL0;
    mpu_initure.AccessPermission = MPU_REGION_FULL_ACCESS;    /* 此region可读写 */
    mpu_initure.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;  /* 允许读取此区域中的指令 */
    mpu_initure.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    mpu_initure.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu_initure.IsBufferable = MPU_ACCESS_BUFFERABLE;
    HAL_MPU_ConfigRegion(&mpu_initure);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);     /* 开启MPU */
}

/**
 * @brief       NAND FALSH底层驱动,引脚配置，时钟使能
 * @note        此函数会被HAL_nand_init()调用
 * @param       hnand;NAND句柄
 * @retval      无
 */
void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_FMC_CLK_ENABLE();             /* 使能FMC时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE();           /* 使能GPIOD时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE();           /* 使能GPIOE时钟 */
    __HAL_RCC_GPIOG_CLK_ENABLE();           /* 使能GPIOG时钟 */

    gpio_init_struct.Pin = GPIO_PIN_6;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;          /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;              /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;         /* 高速 */
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);

    gpio_init_struct.Pin = GPIO_PIN_9;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;          /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;              /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;         /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF12_FMC;       /* 复用为FMC */
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);  

    /* 初始化PD0,1,4,5,11,12,14,15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | \
                     GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_struct.Pull = GPIO_NOPULL;              
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);

    /* 初始化PE7,8,9,10*/
    gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
}

/**
 * @brief       读取NAND FLASH的ID
 * @param       mode:0~5, 表示速度模式
 * @retval      0,成功;1,失败
 */
uint8_t nand_modeset(uint8_t mode)
{   
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_FEATURE;/* 发送设置特性命令 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = 0X01;       /* 地址为0X01,设置mode */
    *(volatile uint8_t*)NAND_ADDRESS = mode;                     /* P1参数,设置mode */
    *(volatile uint8_t*)NAND_ADDRESS = 0;
    *(volatile uint8_t*)NAND_ADDRESS = 0;
    *(volatile uint8_t*)NAND_ADDRESS = 0;

    if (nand_wait_for_ready() == NSTA_READY)
    {
        return 0;        /* 成功 */
    }
    else return 1;       /* 失败 */
}

/**
 * @brief       读取NAND FLASH的ID
 * @note        不同的NAND略有不同，请根据自己所使用的NAND FALSH数据手册来编写函数
 * @param       无
 * @retval      NAND FLASH的ID值
 */
uint32_t nand_readid(void)
{
    uint8_t deviceid[5]; 
    uint32_t id;  
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = nand_readID; /* 发送读取ID命令 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = 0X00;

    /* ID一共有5个字节 */
    deviceid[0] = *(volatile uint8_t*)NAND_ADDRESS;
    deviceid[1] = *(volatile uint8_t*)NAND_ADDRESS;
    deviceid[2] = *(volatile uint8_t*)NAND_ADDRESS;
    deviceid[3] = *(volatile uint8_t*)NAND_ADDRESS;
    deviceid[4] = *(volatile uint8_t*)NAND_ADDRESS;

    /* 镁光的NAND FLASH的ID一共5个字节，但是为了方便我们只取4个字节组成一个32位的ID值
       根据NAND FLASH的数据手册，只要是镁光的NAND FLASH，那么一个字节ID的第一个字节都是0X2C
       所以我们就可以抛弃这个0X2C，只取后面四字节的ID值。*/
    id = ((uint32_t)deviceid[1]) << 24 | ((uint32_t)deviceid[2]) << 16 | ((uint32_t)deviceid[3]) << 8 | deviceid[4];

    return id;
}

/**
 * @brief       读NAND状态
 * @param       无
 * @retval      NAND状态值
 *              bit0:0,成功;1,错误(编程/擦除/READ)
                bit6:0,Busy;1,Ready
 */
uint8_t nand_readstatus(void)
{
    volatile uint8_t data = 0;

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_READSTA;  /* 发送读状态命令 */
    nand_delay(NAND_TWHR_DELAY);                               /* 等待tWHR,再读取状态寄存器 */
    data = *(volatile uint8_t*)NAND_ADDRESS;                       /* 读取状态值 */

    return data;
}

/**
 * @brief       等待NAND准备好
 * @param       无
 * @retval      NSTA_TIMEOUT 等待超时了
 *              NSTA_READY    已经准备好
 */
uint8_t nand_wait_for_ready(void)
{
    uint8_t status = 0;
    volatile uint32_t time = 0;

    while(1)                        /* 等待ready */
    {
        status = nand_readstatus(); /* 获取状态值 */

        if (status & NSTA_READY)break;

        time++;

        if (time >= 0X1FFFFFFF)
        {
            return NSTA_TIMEOUT;    /* 超时 */
        }
    }

    return NSTA_READY;              /* 准备好 */
}

/**
 * @brief       复位NAND
 * @param       无
 * @retval      0,成功; 其他,失败
 */
uint8_t nand_reset(void)
{
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_RESET; /* 复位NAND */

    if (nand_wait_for_ready() == NSTA_READY)
    {
        return 0;                                          /* 复位成功 */
    }

    else return 1;                                         /* 复位失败 */
}

/**
 * @brief       等待RB信号为某个电平
 * @param       rb      : 0,等待RB==0;
 *                        1,等待RB==1;
 * @retval      0,成功; 1,超时
 */
uint8_t nand_waitrb(volatile uint8_t rb)
{
    volatile uint32_t time = 0;
    volatile uint8_t cnt = 0;

    while (time < 0X1FFFFFF)
    {
        time++;

        if (NAND_RB == rb)
        {
            cnt++;
        }
        else
        {
            cnt = 0;
        }
        
        if (cnt > 2)
        {
            return 0; /* 连续三次读取都是正确的有效电平,则认为此次数据有效!(否则-O2优化出问题!) */
        }
    }

    return 1;
}

/**
 * @brief       NAND延时
 * @note        一个i++至少需要4ns
 * @param       i       : 等待的时间
 * @retval      无
 */
void nand_delay(volatile uint32_t i)
{
    while (i > 0)
    {
        i--;
    }
}

/**
 * @brief       读取NAND Flash的指定页指定列的数据(main区和spare区都可以使用此函数)
 * @param       pagenum:要读取的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要读取的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param       *pbuffer:指向数据存储区
 * @param       numbytetoread:读取字节数(不能跨页读)
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_readpage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read)
{
    volatile uint16_t i = 0;
    uint8_t res = 0;
    uint8_t eccnum = 0;     /* 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc */
    uint8_t eccstart = 0;   /* 第一个ECC值所属的地址范围 */
    uint8_t errsta = 0;
    uint8_t *p;

    *(volatile uint8_t*)(NAND_ADDRESS|NAND_CMD) = NAND_AREA_A;
    /* 发送地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_TRUE1;

    /**
     * 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
     * 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
     * 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
     * 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
     * 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
     */
    res = nand_waitrb(0);                                                               /* 等待RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;                  /* 超时退出 */
    }
    /* 下面2行代码是真正判断NAND是否准备好的 */
    res = nand_waitrb(1);                     /* 等待RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;                  /* 超时退出 */
    }

    if (numbyte_to_read % NAND_ECC_SECTOR_SIZE)                                         /* 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验 */
    {
        /* 读取NAND FLASH中的值 */
        for (i = 0; i < numbyte_to_read; i++)
        {
            *(volatile uint8_t*)pbuffer++ = *(volatile uint8_t*)NAND_ADDRESS;
        }
    }
    else
    {
        eccnum = numbyte_to_read / NAND_ECC_SECTOR_SIZE;                                /* 得到ecc计算次数 */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;
        p = pbuffer;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank3->PCR |= 1 << 6;                        /* 使能ECC校验 */

            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)       /* 读取NAND_ECC_SECTOR_SIZE个数据 */
            {
                *(volatile uint8_t*)pbuffer++ = *(volatile uint8_t*)NAND_ADDRESS;
            }

            while(!(FMC_Bank3 -> SR & (1 << 6)));                   /* 等待FIFO空 */

            SCB_CleanInvalidateDCache();                                                /* 清除无效的D-Cache */
            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank3 -> ECCR; /* 读取硬件计算后的ECC值 */
            FMC_Bank3 -> PCR &= ~(1 << 6);                          /* 禁止ECC校验 */
        } 

        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;              /* 从spare区的0X10位置开始读取之前存储的ecc值 */
        nand_delay(NAND_TRHW_DELAY);                                   /* 等待tRHW */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = 0X05;              /* 随机读指令 */
        /* 发送地址 */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = 0XE0;              /* 开始读数据 */

        nand_delay(NAND_TWHR_DELAY);                                   /* 等待tWHR */
        pbuffer = (uint8_t*)&nand_dev.ecc_rdbuf[eccstart];
  
        for (i = 0; i < 4 * eccnum; i++)                               /* 读取保存的ECC值 */
        {
            *(volatile uint8_t*)pbuffer++ = *(volatile uint8_t*)NAND_ADDRESS;
        }
  
        for (i = 0;i < eccnum; i++)                                    /* 检验ECC */
        {
            if (nand_dev.ecc_rdbuf[i + eccstart] != nand_dev.ecc_hdbuf[i + eccstart])  /* 不相等,需要校正 */
            {
                printf("err hd,rd:0x%x,0x%x\r\n", nand_dev.ecc_hdbuf[i + eccstart], nand_dev.ecc_rdbuf[i + eccstart]); 
                printf("eccnum,eccstart:%d,%d\r\n", eccnum, eccstart);
                printf("PageNum,ColNum:%d,%d\r\n", pagenum, colnum);
                res = nand_ecc_correction(p + NAND_ECC_SECTOR_SIZE * i, nand_dev.ecc_rdbuf[i + eccstart], nand_dev.ecc_hdbuf[i + eccstart]);/* ECC校验 */

                if (res)
                {
                    errsta = NSTA_ECC2BITERR;                         /* 标记2BIT及以上ECC错误 */
                }
                else
                {
                    errsta = NSTA_ECC1BITERR;                         /* 标记1BIT ECC错误 */
                }
            }
        }
    }

    if (nand_wait_for_ready() != NSTA_READY)
    {
        errsta = NSTA_ERROR;    /* 失败 */
    }

    return errsta;              /* 成功 */
}

/**
 * @brief       读取NAND Flash的指定页指定列的数据(main区和spare区都可以使用此函数),并对比(FTL管理时需要)
 * @param       pagenum:要读取的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要读取的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param       cmpval:要对比的值,以uint32_t为单位
 * @param       numbytetoread:读取字数(以4字节为单位,不能跨页读)
 * @param       numbyteequal:从初始位置持续与CmpVal值相同的数据个数
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_readpagecomp(uint32_t pagenum, uint16_t colnum, uint32_t cmpval, uint16_t numbyte_to_read, uint16_t *numbyte_equal)
{
    uint16_t i = 0;
    uint8_t res = 0;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_A;
    /* 发送地址 */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_AREA_TRUE1;

    /**
     * 下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
     * 将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
     * 就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
     * 闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
     * 代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
     */
    res = nand_waitrb(0);                                   /* 等待RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;        /* 超时退出 */
    }
    /* 下面2行代码是真正判断NAND是否准备好的 */
    res = nand_waitrb(1);           /* 等待RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;        /* 超时退出 */
    }

    for (i = 0; i < numbyte_to_read; i++)                   /* 读取数据,每次读4字节 */
    {
        if (*(volatile uint32_t*)NAND_ADDRESS != cmpval) break;  /* 如果有任何一个值,与CmpVal不相等,则退出. */
    }

    *numbyte_equal = i;                                     /* 与cmpval值相同的个数 */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;       /* 失败 */
    }

    return 0;                    /* 成功 */
}

/**
 * @brief       在NAND一页中写入指定个字节的数据(main区和spare区都可以使用此函数)
 * @param       pagenum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param       pbuffer:指向数据存储区
 * @param       numbytetowrite:要写入的字节数，该值不能超过该页剩余字节数！！！
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_writepage(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    volatile uint16_t i = 0;  
    uint8_t res = 0;
    uint8_t eccnum = 0;     /* 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc */
    uint8_t eccstart = 0;   /* 第一个ECC值所属的地址范围 */

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    /* 发送地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);
    nand_delay(NAND_TADL_DELAY);               /* 等待tADL */
    
    if (numbyte_to_write % NAND_ECC_SECTOR_SIZE)                        /* 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验 */
    {
        for (i = 0; i < numbyte_to_write; i++)                          /* 写入数据 */
        {
            *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
        }
    }
    else
    {
        eccnum = numbyte_to_write / NAND_ECC_SECTOR_SIZE;               /* 得到ecc计算次数 */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank3 -> PCR |= 1 << 6;                  /* 使能ECC校验 */

            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)   /* 写入NAND_ECC_SECTOR_SIZE个数据 */
            {
                *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
            }
            
            while (!(FMC_Bank3 -> SR & (1 << 6)));       /* 等待FIFO空 */

            SCB_CleanInvalidateDCache();                                /* 清除无效的D-Cache */
            nand_dev.ecc_hdbuf[res+eccstart] = FMC_Bank3 -> ECCR;    /* 读取硬件计算后的ECC值 */
            FMC_Bank3 -> PCR &= ~(1 << 6);               /* 禁止ECC校验 */
        }
        
        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;   /*计算写入ECC的spare区地址 */
        nand_delay(NAND_TADL_DELAY);                        /* 等待tADL */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = 0X85;   /* 随机写指令 */

        /* 发送地址 */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        nand_delay(NAND_TADL_DELAY);                        /* 等待tADL */
        pbuffer = (uint8_t*)&nand_dev.ecc_hdbuf[eccstart];

        for (i = 0; i < eccnum; i++)                        /* 写入ECC */
        { 
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
            }
        }
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1; 
    delay_us(NAND_TPROG_DELAY);     /* 等待tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;          /* 失败 */
    }

    return 0;                       /* 成功 */
}

/**
 * @brief       在NAND一页中的指定地址开始,写入指定长度的恒定数字
 * @param       pagenum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要写入的列开始地址(也就是页内地址),范围:0~(page_totalsize-1)
 * @param       cval:要写入的指定常数
 * @param       numbytetowrite:要写入的字节数(以4字节为单位)
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_write_pageconst(uint32_t pagenum, uint16_t colnum, uint32_t cval, uint16_t numbyte_to_write)
{
    uint16_t i = 0;  
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE0;
    /* 发送地址 */
    *(volatile uint8_t*)(NAND_ADDRESS |NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(pagenum >> 16);

    nand_delay(NAND_TADL_DELAY);                /* 等待tADL */

    for (i = 0; i < numbyte_to_write; i++)      /* 写入数据,每次写4字节 */
    {
        *(volatile uint32_t*)NAND_ADDRESS = cval;
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_WRITE_TURE1; 
    delay_us(NAND_TPROG_DELAY);                 /* 等待tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;                      /* 失败  */
    }

    return 0;                                   /* 成功 */
}

/**
 * @brief       将一页数据拷贝到另一页,不写入新数据
 * @note        源页和目的页要在同一个Plane内！
 * @param       source_pagenum:源页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       dest_pagenum:目的页地址,范围:0~(block_pagenum*block_totalnum-1) 
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_copypage_withoutwrite(uint32_t source_pagenum, uint32_t dest_pagenum)
{
    uint8_t res = 0;
    uint16_t source_block = 0;
    uint16_t dest_block = 0;
    /* 判断源页和目的页是否在同一个plane中 */
    source_block = source_pagenum / nand_dev.block_pagenum;
    dest_block = dest_pagenum / nand_dev.block_pagenum;

    if ((source_block % 2) != (dest_block % 2))
    {
        return NSTA_ERROR;  /* 不在同一个plane内 */
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0; /* 发送命令0X00 */
    /* 发送源页地址 */
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)source_pagenum;
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 8);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 16);
    *(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD1; /* 发送命令0X35 */
    /**
     *   下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
     *   将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
     *   就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
     *   闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
     *   代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
    */
    res = nand_waitrb(0);        /* 等待RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;     /* 超时退出 */
    }
    /* 下面2行代码是真正判断NAND是否准备好的 */
    res = nand_waitrb(1);        /* 等待RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;    /* 超时退出 */
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2;  /* 发送命令0X85 */
    /* 发送目的页地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)dest_pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 16);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD3;  /* 发送命令0X10 */
    delay_us(NAND_TPROG_DELAY);     /* 等待tPROG  */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;          /* NAND未准备好 */
    }

    return 0;                       /* 成功 */
}

/**
 * @brief       将一页数据拷贝到另一页,并且可以写入数据
 * @note        源页和目的页要在同一个Plane内！
 * @param       source_pagenum:源页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       dest_pagenum:目的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:页内列地址,范围:0~(page_totalsize-1)
 * @param       pbuffer:要写入的数据
 * @param       numbytetowrite:要写入的数据个数
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_copypage_withwrite(uint32_t source_pagenum, uint32_t dest_pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    uint8_t res = 0;
    volatile uint16_t i = 0;
    uint16_t source_block = 0, dest_block = 0;  
    uint8_t eccnum = 0;         /* 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc */
    uint8_t eccstart = 0;       /* 第一个ECC值所属的地址范围 */
    /* 判断源页和目的页是否在同一个plane中 */
    source_block = source_pagenum / nand_dev.block_pagenum;
    dest_block = dest_pagenum / nand_dev.block_pagenum;

    if ((source_block % 2) != (dest_block % 2))
    {
        return NSTA_ERROR;      /* 不在同一个plane内 */
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD0;  /* 发送命令0X00 */
    /* 发送源页地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)0;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)source_pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(source_pagenum >> 16);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD1;  /* 发送命令0X35 */

/**
 *    下面两行代码是等待R/B引脚变为低电平，其实主要起延时作用的，等待NAND操作R/B引脚。因为我们是通过
 *    将STM32的NWAIT引脚(NAND的R/B引脚)配置为普通IO，代码中通过读取NWAIT引脚的电平来判断NAND是否准备
 *    就绪的。这个也就是模拟的方法，所以在速度很快的时候有可能NAND还没来得及操作R/B引脚来表示NAND的忙
 *    闲状态，结果我们就读取了R/B引脚,这个时候肯定会出错的，事实上确实是会出错!大家也可以将下面两行
 *    代码换成延时函数,只不过这里我们为了效率所以没有用延时函数。
*/
    res = nand_waitrb(0);       /* 等待RB=0 */

    if (res)
    {
        return NSTA_TIMEOUT;    /* 超时退出 */
    }
    /* 下面2行代码是真正判断NAND是否准备好的 */
    res = nand_waitrb(1);       /* 等待RB=1 */

    if (res)
    {
        return NSTA_TIMEOUT;    /* 超时退出 */
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD2;  /* 发送命令0X85 */
    /* 发送目的页地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)colnum;
    *(volatile uint8_t*)(NAND_ADDRESS  |NAND_ADDR) = (uint8_t)(colnum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)dest_pagenum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(dest_pagenum >> 16); 
    /* 发送页内列地址 */
    nand_delay(NAND_TADL_DELAY);               /* 等待tADL */

    if (numbyte_to_write % NAND_ECC_SECTOR_SIZE)                        /* 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验 */
    {  
        for (i = 0; i < numbyte_to_write; i++)                          /* 写入数据 */
        {
            *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
        }
    }
    else
    {
        eccnum = numbyte_to_write / NAND_ECC_SECTOR_SIZE;               /* 得到ecc计算次数 */
        eccstart = colnum / NAND_ECC_SECTOR_SIZE;

        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank3 -> PCR |= 1 << 6;                     /* 使能ECC校验 */
 
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++)      /* 写入NAND_ECC_SECTOR_SIZE个数据 */
            {
                *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
            }
            
            while (!(FMC_Bank3 -> SR & (1<<6)));            /* 等待FIFO空 */

            SCB_CleanInvalidateDCache();                                /* 清除无效的D-Cache */
            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank3 -> ECCR;  /* 读取硬件计算后的ECC值 */

            FMC_Bank3 -> PCR &= ~(1 << 6);                  /* 禁止ECC校验 */
        }

        i = nand_dev.page_mainsize + 0X10 + eccstart * 4;   /* 计算写入ECC的spare区地址 */
        nand_delay(NAND_TADL_DELAY);                        /* 等待tADL */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = 0X85;   /* 随机写指令 */
        /* 发送地址 */
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)i;
        *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(i >> 8);
        nand_delay(NAND_TADL_DELAY);                        /*等待tADL */

        pbuffer = (uint8_t*)&nand_dev.ecc_hdbuf[eccstart];

        for (i = 0; i < eccnum; i++)                        /* 写入ECC */
        { 
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t*)NAND_ADDRESS = *(volatile uint8_t*)pbuffer++;
            }
        }
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_MOVEDATA_CMD3; /* 发送命令0X10 */
    delay_us(NAND_TPROG_DELAY);                                     /* 等待tPROG */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;  /* 失败 */
    }

    return 0;               /* 成功 */ 
}

/**
 * @brief       读取spare区中的数据
 * @param       pagenum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要写入的spare区地址(spare区中哪个地址),范围:0~(page_sparesize-1) 
 * @param       pbuffer:接收数据缓冲区 
 * @param       numbytetoread:要读取的字节数(不大于page_sparesize)
 * @retval      0,成功;其他,错误代码
 */
uint8_t nand_readspare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_read)
{
    uint8_t temp = 0;
    uint8_t remainbyte = 0;
    remainbyte = nand_dev.page_sparesize - colnum;

    if (numbyte_to_read > remainbyte)
    {
        numbyte_to_read = remainbyte;                                                           /* 确保要写入的字节数不大于spare剩余的大小 */
    }

    temp = nand_readpage(pagenum, colnum + nand_dev.page_mainsize, pbuffer, numbyte_to_read);   /* 读取数据 */
    return temp;
}

/**
 * @brief       向spare区中写数据
 * @param       pagenum:要写入的页地址,范围:0~(block_pagenum*block_totalnum-1)
 * @param       colnum:要写入的spare区地址(spare区中哪个地址),范围:0~(page_sparesize-1) 
 * @param       pbuffer:要写入的数据首地址  
 * @param       numbytetowrite:要写入的字节数(不大于page_sparesize)
 * @retval      0,成功;其他,失败
 */
uint8_t nand_writespare(uint32_t pagenum, uint16_t colnum, uint8_t *pbuffer, uint16_t numbyte_to_write)
{
    uint8_t temp = 0;
    uint8_t remainbyte = 0;

    remainbyte = nand_dev.page_sparesize - colnum;

    if (numbyte_to_write > remainbyte)
    {
        numbyte_to_write = remainbyte;                                                          /* 确保要读取的字节数不大于spare剩余的大小 */
    }

    temp = nand_writepage(pagenum, colnum + nand_dev.page_mainsize, pbuffer, numbyte_to_write); /* 读取 */

    return temp;
}

/**
 * @brief       擦除一个块
 * @param       blocknum:要擦除的BLOCK编号,范围:0-(block_totalnum-1)
 * @retval      返回值:0,擦除成功;其他,擦除失败
 */
uint8_t nand_eraseblock(uint32_t blocknum)
{
    if (nand_dev.id == MT29F16G08ABABA)
    {
        blocknum <<= 7;             /* 将块地址转换为页地址 */
    }
    else if (nand_dev.id == MT29F4G08ABADA)
    {
        blocknum <<= 6;
    }

    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_ERASE0;
    /* 发送块地址 */
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)blocknum;
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(blocknum >> 8);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_ADDR) = (uint8_t)(blocknum >> 16);
    *(volatile uint8_t*)(NAND_ADDRESS | NAND_CMD) = NAND_ERASE1;

    delay_ms(NAND_TBERS_DELAY);     /* 等待擦除成功 */

    if (nand_wait_for_ready() != NSTA_READY)
    {
        return NSTA_ERROR;          /* 失败 */
    }

    return 0;                       /* 成功 */
}

/**
 * @brief       全片擦除NAND FLASH
 * @param       无
 * @retval      无
 */
void nand_erasechip(void)
{
    uint8_t status;
    uint16_t i = 0;

    for (i = 0; i < nand_dev.block_totalnum; i++)   /* 循环擦除所有的块 */
    {
        status = nand_eraseblock(i);

        if (status)
        {
            printf("Erase %d block fail!!，错误码为%d\r\n",i,status);  /* 擦除失败 */
        }
    }
}

/**
 * @brief       获取ECC的奇数位/偶数位
 * @param       oe:0,偶数位; 1,奇数位
 * @param       eccval:输入的ecc值
 * @retval      计算后的ecc值(最多16位)
 */
uint16_t nand_ecc_get_oe(uint8_t oe, uint32_t eccval)
{
    uint8_t i;
    uint16_t ecctemp = 0;

    for (i = 0; i < 24; i++)
    {
        if ((i % 2) == oe)
        {
            if ((eccval >> i) & 0X01)
            {
                ecctemp += 1 << (i >> 1); 
            }
        }
    }

    return ecctemp;
}

/**
 * @brief       ECC校正函数
 * @param       data_buf:数据缓存区
 * @param       eccrd:读取出来,原来保存的ECC值
 * @param       ecccl:读取数据时,硬件计算的ECC值
 * @retval      0,错误已修正;其他,ECC错误(有大于2个bit的错误,无法恢复)
 */
uint8_t nand_ecc_correction(uint8_t* data_buf, uint32_t eccrd, uint32_t ecccl)
{
    uint16_t eccrdo, eccrde, eccclo, ecccle;
    uint16_t eccchk = 0;
    uint16_t errorpos = 0;
    uint32_t bytepos = 0;
    eccrdo = nand_ecc_get_oe(1, eccrd); /* 获取eccrd的奇数位 */
    eccrde = nand_ecc_get_oe(0, eccrd); /* 获取eccrd的偶数位 */
    eccclo = nand_ecc_get_oe(1, ecccl); /* 获取ecccl的奇数位 */
    ecccle = nand_ecc_get_oe(0, ecccl); /* 获取ecccl的偶数位 */
    eccchk = eccrdo^eccrde^eccclo^ecccle;

    if (eccchk == 0XFFF)                /* 全1,说明只有1bit ECC错误 */
    {
        errorpos = eccrdo^eccclo; 
        printf("errorpos:%d\r\n", errorpos); 
        bytepos = errorpos / 8; 
        data_buf[bytepos] ^= 1 << (errorpos % 8);
    }
    else                                /* 不是全1,说明至少有2bit ECC错误,无法修复 */
    {
        printf("2bit ecc error or more\r\n");
        return 1;
    } 

    return 0;
}
 






