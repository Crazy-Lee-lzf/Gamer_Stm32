/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-07-19
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
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
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* 设置栈顶地址 */
}

/**
 * @brief       使能STM32F7的L1-Cache, 同时开启D cache的强制透写
 * @param       无
 * @retval      无
 */
void sys_cache_enable(void)
{
    SCB_EnableICache(); /* 使能I-Cache,函数在core_cm7.h里面定义 */
    SCB_EnableDCache(); /* 使能D-Cache,函数在core_cm7.h里面定义 */
    SCB->CACR |= 1 << 2;/* 强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题 */
}

/**
 * @brief       判断I_Cache是否打开
 * @param       无
 * @retval      返回值:0 关闭，1 打开
 */
uint8_t get_icache_sta(void)
{
    uint8_t sta;
    sta = ((SCB->CCR) >> 17) & 0X01;
    return sta;
}

/**
 * @brief       判断D_Cache是否打开
 * @param       无
 * @retval      返回值:0 关闭，1 打开
 */
uint8_t get_dcache_sta(void)
{
    uint8_t sta;
    sta = ((SCB->CCR) >> 16) & 0X01;
    return sta;
}

/**
 * @brief       时钟设置函数
 * @param       plln: PLL1倍频系数(PLL倍频), 取值范围: 192~432.
 * @param       pllm: PLL1预分频系数(进PLL之前的分频), 取值范围: 2~63.
 * @param       pllp: PLL1的p分频系数(PLL之后的分频), 分频后作为系统时钟, 取值范围: 2,4,6,8.(仅限这4个值!)
 * @param       pllq: PLL1的q分频系数(PLL之后的分频), 取值范围: 2~15.
 * @note
 *
 *              Fvco: VCO频率
 *              Fsys: 系统时钟频率, 也是PLL1的p分频输出时钟频率
 *              Fq:   PLL1的q分频输出时钟频率
 *              Fs:   PLL输入时钟频率, 可以是HSI, CSI, HSE等.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              外部晶振为25M的时候, 推荐值: plln = 432, pllm = 25, pllp = 2, pllq = 9.
 *              得到:Fvco = 25 * ( 432 / 25) = 432Mhz
 *                   Fsys = pll1_p_ck = 432 / 2 = 216Mhz
 *                   Fq   = pll1_q_ck = 432 / 9 = 48Mhz
 *
 *              F767默认需要配置的频率如下:
 *              CPU频率(rcc_c_ck) = sys_d1cpre_ck = 216Mhz
 *              AHB1/2/3(rcc_hclk1/2/3) = 216Mhz
 *              APB1(rcc_pclk1) = pll_p_ck / 4 = 54Mhz
 *              APB2(rcc_pclk2) = pll_p_ck / 2 = 108Mhz
 *
 * @retval      错误代码: 0, 成功; 1, 错误;
 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef rcc_osc_init = {0};
    RCC_ClkInitTypeDef rcc_clk_init = {0};
  
    __HAL_RCC_PWR_CLK_ENABLE();                                            /* 使能PWR时钟 */

    /* 下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作时使性能与功耗实现平衡 */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  /* VOS = 1, Scale1, 1.2V内核电压,FLASH访问可以得到最高性能 */

    /* 使能HSE，并选择HSE作为PLL时钟源，配置PLL1，开启USB时钟 */
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;           /* 时钟源为HSE */
    rcc_osc_init.HSEState = RCC_HSE_ON;                             /* 打开HSE */
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                         /* 打开PLL */
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;                 /* PLL时钟源选择HSE */
    rcc_osc_init.PLL.PLLN = plln;
    rcc_osc_init.PLL.PLLM = pllm;
    rcc_osc_init.PLL.PLLP = pllp;
    rcc_osc_init.PLL.PLLQ = pllq;
    ret = HAL_RCC_OscConfig(&rcc_osc_init);
    if(ret != HAL_OK)
    {
        return 1;
    }
    
    ret = HAL_PWREx_EnableOverDrive();   /* 开启Over-Driver功能 */
    if(ret != HAL_OK)
    {
        return 1;
    }

    /*
     *  选择PLL的输出作为系统时钟
     *  配置RCC_CLOCKTYPE_SYSCLK系统时钟,216M
     *  配置RCC_CLOCKTYPE_HCLK 时钟,216hz,对应AHB1，AHB2，AHB3总线
     *  配置RCC_CLOCKTYPE_PCLK1时钟,54Mhz,对应APB1总线
     *  配置RCC_CLOCKTYPE_PCLK2时钟,108Mhz,对应APB2总线
     */

    rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK \
                                    | RCC_CLOCKTYPE_HCLK \
                                    | RCC_CLOCKTYPE_PCLK1 \
                                    | RCC_CLOCKTYPE_PCLK2);

    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;            /* 设置系统时钟时钟源为PLL */
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;                   /* AHB分频系数为1 */
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV4;                    /* APB1分频系数为4 */
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV2;                    /* APB2分频系数为2 */
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_7);      /* 同时设置FLASH延时周期为7WS */
    if(ret != HAL_OK)
    {
        return 1;                                                   /* 时钟初始化失败 */
    }
    return 0;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief       当编译提示出错的时候此函数用来报告错误的文件和所在行
 * @param       file：指向源文件
 *              line：指向在文件中的行数
 * @retval      无
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

