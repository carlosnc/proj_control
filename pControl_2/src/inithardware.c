/*
 ===============================================================================
 Name        : inithardware.c
 Author      : $(author)
 Version     : 0.0-alpha1
 Copyright   : $(copyright)
 Description : Start sequence for EDU-CIAA.
 Note        : All translations were made using the Google translator. XD.
 ===============================================================================
 */

#include "inithardware.h"
#include "edu_ciaa_i2c.h"
#include "edu_ciaa_uart.h"

// Private functions prototypes ================================================
static void initSystemClock(void);
static void initLEDs(void);
static void initButtons(void);

// Public functions ============================================================
void initHardware_Init(void)
{
  initSystemClock();
  SystemCoreClockUpdate();                              //System clock 208 MHz

  SysTick_Config(SystemCoreClock / 1000u);              //1ms SysTick
  NVIC_SetPriority(SysTick_IRQn, ((0x01 << 0) - 0x01));

  initLEDs();
  initButtons();

  ciaa_uart_init_t uart_initStruct;
  uart_initStruct.ID = CIAA_UART3;
  uart_initStruct.Mode = UART_MODE_NORMAL;
  uart_initStruct.BaudRate = 115200;
  uart_initStruct.DataConfig = UART_CONFIG_8N1;
  ciaa_uart_init(&uart_initStruct);

  ciaa_I2C_init(I2C0, I2C_MODE_POLLING, 100000);
}

void initHardware_testOutputs(void)
{
  uint8_t idx = 0;
  const ciaa_gpio_t *Leds[6] = { &LED_RGB_Rojo, &LED_RGB_Verde, &LED_RGB_Azul,
      &LED_Rojo, &LED_Amarillo, &LED_Verde };

  for (uint8_t i = 0; i < 12; i++)
  {
    ciaa_togglePin(Leds[idx]);
    idx = (i == 5) ? (0) : (idx + 1);
    pauseMs(200);
  }
}

// Private functions ===========================================================
static void initSystemClock(void)
{
  __NOP();
}

static void initLEDs(void)
{
  ciaa_initLED(&LED_RGB_Rojo);
  ciaa_initLED(&LED_RGB_Verde);
  ciaa_initLED(&LED_RGB_Azul);

  ciaa_setPinLow(&LED_RGB_Rojo);
  ciaa_setPinLow(&LED_RGB_Verde);
  ciaa_setPinLow(&LED_RGB_Azul);

  ciaa_initLED(&LED_Rojo);
  ciaa_initLED(&LED_Amarillo);
  ciaa_initLED(&LED_Verde);

  ciaa_setPinLow(&LED_Rojo);
  ciaa_setPinLow(&LED_Amarillo);
  ciaa_setPinLow(&LED_Verde);
}

static void initButtons(void)
{
  ciaa_initTec(&TEC01, CIAA_TEC_INTERRUPT);
  ciaa_initTec(&TEC04, CIAA_TEC_INTERRUPT);
}

// EOF =========================================================================
