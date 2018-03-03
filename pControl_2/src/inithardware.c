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
static void initPeripherals_and_Sensor(void);

// Public functions ============================================================
void initHardware_Init(void)
{
  initSystemClock();
  SystemCoreClockUpdate();                              //System clock 208 MHz

  SysTick_Config(SystemCoreClock / 1000u);              //1ms SysTick
  NVIC_SetPriority(SysTick_IRQn, ((0x01 << 0) - 0x01));

  initLEDs();
  initButtons();
  initPeripherals_and_Sensor();
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
}

static void initPeripherals_and_Sensor(void)
{
  ciaa_uart_init_t uart_initStruct;
  uart_initStruct.ID = CIAA_UART3;
  uart_initStruct.Mode = UART_MODE_NORMAL;
  uart_initStruct.BaudRate = 115200;
  uart_initStruct.DataConfig = UART_CONFIG_8N1;
  ciaa_uart_init(&uart_initStruct);

  ciaa_I2C_init(I2C0, I2C_MODE_POLLING, 100000);

  mpu9250_InitStruct_t mpu9250_InitStruct;
  mpu9250_InitStruct.Accel_Scale = MPU9250_ACCEL_FULLSCALE_2G;
  mpu9250_InitStruct.Accel_Axes  = MPU9250_ACCEL_XYZ_ENABLE;
  mpu9250_InitStruct.Accel_LPF   = MPU9250_ACCEL_LPF_44_8HZ;
  mpu9250_InitStruct.Gyro_Scale  = MPU9250_GYRO_FULLSCALE_250DPS;
  mpu9250_InitStruct.Gyro_Axes   = MPU9250_GYRO_XYZ_ENABLE;
  mpu9250_InitStruct.Gyro_LPF    = MPU9250_GYRO_LPF_41HZ;
  mpu9250_InitStruct.SampleRate  = 99;
  if( mpu9250_init(&mpu9250_InitStruct) == MPU9250_OK)
    ciaa_blinkPin(&LED_Verde);

  servo_initStruct_t servo_initStruct;
  servo_initStruct.servo_channel = SERVO_CHANNEL_ALL;
  servo_initStruct.angle_min = -60;
  servo_initStruct.angle_max =  60;
  servo_initStruct.pos_zero  =  90;
  ciaa_servo_init(&servo_initStruct);

  filter_initStruct_t filter_initStruct;
  filter_initStruct.freq_update = 100;
  filter_initStruct.weight_of_filter = 0.99;
  ciaa_filter_init(&filter_initStruct);
}

// EOF =========================================================================
