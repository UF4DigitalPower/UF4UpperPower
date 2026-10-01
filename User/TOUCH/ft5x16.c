#include "ft5x16.h"

#include "gpio.h"
#include "i2c.h"

#define FT5X16_I2C_ADDR_38              0x70U
#define FT5X16_I2C_ADDR_39              0x72U

#define FT5X16_REG_DEV_MODE             0x00U
#define FT5X16_REG_GEST_ID              0x01U
#define FT5X16_REG_TD_STATUS            0x02U
#define FT5X16_REG_TOUCH1_XH            0x03U
#define FT5X16_REG_TOUCH1_XL            0x04U
#define FT5X16_REG_TOUCH1_YH            0x05U
#define FT5X16_REG_TOUCH1_YL            0x06U

#define FT5X16_REG_G_MODE               0xA4U
#define FT5X16_REG_THGROUP              0x80U
#define FT5X16_REG_PERIOD_ACTIVE        0x88U
#define FT5X16_REG_LIB_VERSION          0xA1U

#define FT5X16_I2C_TIMEOUT_MS           20U

static uint16_t g_ft5x16_i2c_addr = FT5X16_I2C_ADDR_38;

static HAL_StatusTypeDef ft5x16_write(uint8_t reg, const uint8_t *data, uint16_t len)
{

    return HAL_I2C_Mem_Write(&hi2c1,
                             g_ft5x16_i2c_addr,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             (uint8_t *)data,
                             len,
                             FT5X16_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef ft5x16_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1,
                            g_ft5x16_i2c_addr,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            data,
                            len,
                            FT5X16_I2C_TIMEOUT_MS);
}

static void ft5x16_gpio_prepare(uint8_t int_level)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin = TOUCH_INT_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TOUCH_INT_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TOUCH_INT_GPIO_Port, TOUCH_INT_Pin,
                      (int_level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    gpio.Pin = TPUCH_RST_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TPUCH_RST_GPIO_Port, &gpio);
}

static void ft5x16_gpio_release_int(void)
{
    GPIO_InitTypeDef gpio = {0};

    gpio.Pin = TOUCH_INT_Pin;
    /* FT5X16 signals an active-low touch interrupt.  This must restore EXTI
       after the address-select reset sequence, otherwise GUI only observes
       the pin level and pulse-style interrupts are lost. */
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TOUCH_INT_GPIO_Port, &gpio);
}

static void ft5x16_hardware_reset(uint8_t int_level)
{
    ft5x16_gpio_prepare(int_level);
    HAL_GPIO_WritePin(TPUCH_RST_GPIO_Port, TPUCH_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(20U);
    HAL_GPIO_WritePin(TPUCH_RST_GPIO_Port, TPUCH_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(20U);
    ft5x16_gpio_release_int();
    HAL_Delay(50U);
}

static HAL_StatusTypeDef ft5x16_probe(uint16_t dev_addr)
{
    uint32_t trials = 0U;

    while (trials < 3U)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c1, dev_addr, 1U, FT5X16_I2C_TIMEOUT_MS) == HAL_OK)
        {
            g_ft5x16_i2c_addr = dev_addr;
            return HAL_OK;
        }
        ++trials;
    }

    return HAL_ERROR;
}

static void ft5x16_configure(void)
{
    uint8_t value;

    value = 0U;
    (void)ft5x16_write(FT5X16_REG_DEV_MODE, &value, 1U);
    (void)ft5x16_write(FT5X16_REG_G_MODE, &value, 1U);

    value = 22U;
    (void)ft5x16_write(FT5X16_REG_THGROUP, &value, 1U);

    value = 12U;
    (void)ft5x16_write(FT5X16_REG_PERIOD_ACTIVE, &value, 1U);

    (void)ft5x16_read(FT5X16_REG_LIB_VERSION, &value, 1U);
    (void)ft5x16_read(FT5X16_REG_GEST_ID, &value, 1U);
}

HAL_StatusTypeDef FT5X16_Init(void)
{
    static const uint16_t address_candidates[] =
    {
        FT5X16_I2C_ADDR_38,
        FT5X16_I2C_ADDR_39
    };
    uint32_t polarity;
    uint32_t index;

    for (polarity = 0U; polarity < 2U; ++polarity)
    {
        ft5x16_hardware_reset((uint8_t)polarity);

        for (index = 0U; index < (uint32_t)(sizeof(address_candidates) / sizeof(address_candidates[0])); ++index)
        {
            if (ft5x16_probe(address_candidates[index]) == HAL_OK)
            {
                ft5x16_configure();
                return HAL_OK;
            }
        }
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef FT5X16_ReadPoints(FT5X16_Point_t *points, uint8_t *count)
{
    uint8_t status = 0U;
    uint8_t raw[FT5X16_MAX_POINTS * 6U];
    uint8_t point_count;
    uint8_t i;

    if (count == NULL)
    {
        return HAL_ERROR;
    }

    *count = 0U;
    if (points == NULL)
    {
        return HAL_ERROR;
    }

    if (ft5x16_read(FT5X16_REG_TD_STATUS, &status, 1U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    point_count = (uint8_t)(status & 0x0FU);
    if (point_count == 0U)
    {
        return HAL_OK;
    }

    if (point_count > FT5X16_MAX_POINTS)
    {
        point_count = FT5X16_MAX_POINTS;
    }

    /* The point records are contiguous.  Reading the whole block in one
     * transaction removes one I2C start/stop pair from the common one-point
     * path and reduces the time before a button press reaches LVGL. */
    if (ft5x16_read(FT5X16_REG_TOUCH1_XH, raw, (uint16_t)(point_count * 6U)) != HAL_OK)
    {
        return HAL_ERROR;
    }

    for (i = 0U; i < point_count; ++i)
    {
        const uint8_t *record = &raw[i * 6U];

        points[i].event = (uint8_t)((record[0] >> 6) & 0x03U);
        points[i].x = (uint16_t)(((uint16_t)(record[0] & 0x0FU) << 8) | record[1]);
        points[i].y = (uint16_t)(((uint16_t)(record[2] & 0x0FU) << 8) | record[3]);
        points[i].id = (uint8_t)((record[2] >> 4) & 0x0FU);
    }

    *count = point_count;
    return HAL_OK;
}

uint8_t FT5X16_IsTouched(void)
{
    return (uint8_t)(HAL_GPIO_ReadPin(TOUCH_INT_GPIO_Port, TOUCH_INT_Pin) == GPIO_PIN_RESET);
}
