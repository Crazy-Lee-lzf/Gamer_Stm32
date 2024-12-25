/**
 ****************************************************************************************************
 * @file        sdmmc_sdcard.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-07-19
 * @brief       SD�� ��������
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
 * �޸�˵��
 * V1.0 20200719
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"


SD_HandleTypeDef g_sd_handle;                     /* SD����� */
HAL_SD_CardInfoTypeDef g_sd_card_info_struct;     /* SD����Ϣ�ṹ�� */

/**
 * @brief       SD����ʼ��
 * @param       ��
 * @retval      ����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;
    
    g_sd_handle.Instance = SDMMC1;
    g_sd_handle.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;                         /* ������ */
    g_sd_handle.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;                    /* ��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDMMC_CLK */
    g_sd_handle.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;             /* ����ʱ���ر�ʱ�ӵ�Դ */
    g_sd_handle.Init.BusWide = SDMMC_BUS_WIDE_1B;                                 /* 1λ������ */
    g_sd_handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;   /* �ر�Ӳ�������� */
    g_sd_handle.Init.ClockDiv = SDMMC_TRANSFER_CLK_DIV;                           /* SD����ʱ��Ƶ�����25MHZ */
    
    SD_Error = HAL_SD_Init(&g_sd_handle);
    if (SD_Error != HAL_OK)
    {
        return 1;
    }

    HAL_SD_GetCardInfo(&g_sd_handle, &g_sd_card_info_struct);   /* ��ȡSD����Ϣ */
    
    SD_Error = HAL_SD_ConfigWideBusOperation(&g_sd_handle, SDMMC_BUS_WIDE_4B);   /* ��������Ϊ4bit������ģʽ */
    if (SD_Error != HAL_OK)
    {
        return 2;
    }
    
    return 0;
}

/**
 * @brief       SDMMC�ײ�������ʱ��ʹ�ܣ��������ã�DMA����
 * @param       hsd:SD�����
 * @note        �˺����ᱻHAL_SD_Init()����
 * @retval      ��
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_SDMMC1_CLK_ENABLE();  /* ʹ��SDMMC1ʱ�� */
    SD1_D0_GPIO_CLK_ENABLE();       /* D0����IOʱ��ʹ�� */
    SD1_D1_GPIO_CLK_ENABLE();       /* D1����IOʱ��ʹ�� */
    SD1_D2_GPIO_CLK_ENABLE();       /* D2����IOʱ��ʹ�� */
    SD1_D3_GPIO_CLK_ENABLE();       /* D3����IOʱ��ʹ�� */
    SD1_CLK_GPIO_CLK_ENABLE();      /* CLK����IOʱ��ʹ�� */
    SD1_CMD_GPIO_CLK_ENABLE();      /* CMD����IOʱ��ʹ�� */

    gpio_init_struct.Pin = SD1_D0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ���츴�� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* ���� */
    gpio_init_struct.Alternate = GPIO_AF12_SDIO;            /* ����ΪSDIO */
    HAL_GPIO_Init(SD1_D0_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��D0���� */

    gpio_init_struct.Pin = SD1_D1_GPIO_PIN;
    HAL_GPIO_Init(SD1_D1_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��D1���� */

    gpio_init_struct.Pin = SD1_D2_GPIO_PIN;
    HAL_GPIO_Init(SD1_D2_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��D2���� */

    gpio_init_struct.Pin = SD1_D3_GPIO_PIN;
    HAL_GPIO_Init(SD1_D3_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��D3���� */

    gpio_init_struct.Pin = SD1_CLK_GPIO_PIN;
    HAL_GPIO_Init(SD1_CLK_GPIO_PORT, &gpio_init_struct);    /* ��ʼ��CLK���� */

    gpio_init_struct.Pin = SD1_CMD_GPIO_PIN;
    HAL_GPIO_Init(SD1_CMD_GPIO_PORT, &gpio_init_struct);    /* ��ʼ��CMD���� */
}

/**
 * @brief       ��ȡ����Ϣ����
 * @param       cardinfo:SD����Ϣ���
 * @retval      ��ȡ����Ϣ״ֵ̬
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&g_sd_handle, cardinfo);
    return sta;
}

/**
 * @brief       �ж�SD���Ƿ���Դ���(��д)����
 * @param       ��
 * @retval      ����ֵ:SD_TRANSFER_OK      ������ɣ����Լ�����һ�δ���
                       SD_TRANSFER_BUSY SD ����æ�������Խ�����һ�δ���
 */
uint8_t get_sd_card_state(void)
{
  return ((HAL_SD_GetCardState(&g_sd_handle) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       ��SD������
 * @param       buf:�����ݻ�����
 * @param       sector:������ַ
 * @param       cnt:��������
 * @retval      ����ֵ:0,����;����,����;
 */
uint8_t sd_read_disk(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;
    sys_intx_disable();        /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����!!!) */
    sta = HAL_SD_ReadBlocks(&g_sd_handle, (uint8_t*)buf,lsector, cnt, SD_TIMEOUT); /* ���sector�Ķ����� */

    /* �ȴ�SD������ */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }

    sys_intx_enable();  /* �������ж� */
    return sta;
}

/**
 * @brief       д���ݵ�SD��
 * @param       buf:д���ݻ�����
 * @param       sector:������ַ
 * @param       cnt:��������
 * @retval      ����ֵ:0,����;����,����;
 */
uint8_t sd_write_disk(uint8_t *buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;
    sys_intx_disable();  /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����!!!) */
    sta = HAL_SD_WriteBlocks(&g_sd_handle, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT); /* ���sector��д���� */
        
    /* �ȴ�SD��д�� */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }

    sys_intx_enable(); /* �������ж� */
    return sta;
}






