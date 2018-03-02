/*
 ===============================================================================
 Name        : hd44780.c
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
  uart_initStruct.Mode = UART_MODE_INTERRUPT;
  uart_initStruct.BaudRate = 115200;
  uart_initStruct.DataConfig = UART_CONFIG_8N1;
  ciaa_uart_init(&uart_initStruct);

  ciaa_I2C_init(I2C0, I2C_MODE_INTERRUPT, 100000);
}

void initHardware_testOutputs(void)
{
  uint8_t idx = 0;
  const ciaa_gpio_t *Leds[6] = { &LED_RGB_Rojo, &LED_RGB_Verde, &LED_RGB_Azul,
      &LED_Rojo, &LED_Amarillo, &LED_Verde };

  for (uint8_t i = 0; i < 12; i++)
  {
    setPinToggle(Leds[idx]);
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
  initLED(&LED_RGB_Rojo);
  initLED(&LED_RGB_Verde);
  initLED(&LED_RGB_Azul);

  setPinLow(&LED_RGB_Rojo);
  setPinLow(&LED_RGB_Verde);
  setPinLow(&LED_RGB_Azul);

  initLED(&LED_Rojo);
  initLED(&LED_Amarillo);
  initLED(&LED_Verde);

  setPinLow(&LED_Rojo);
  setPinLow(&LED_Amarillo);
  setPinLow(&LED_Verde);
}

static void initButtons(void)
{
  initTec(&TEC01, CIAA_TEC_INTERRUPT);
  initTec(&TEC04, CIAA_TEC_INTERRUPT);
}

// EOF =========================================================================
