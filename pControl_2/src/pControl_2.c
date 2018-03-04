#include "inithardware.h"
#include "stdlib.h"

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types";

// Global variables ============================================================
volatile ciaa_app_state_t appState = APP_STOPPED;
volatile uint8_t cycle = 0;

float32_t temperature = 0x0000;
float32_t *pTemperature = &temperature;

/*
 * aAngles[0]: Pitch angle
 * aAngles[1]: Roll  angle
 */
float32_t aAngles[2] = { 0.0f };
float32_t *pAngles = &aAngles[0];

// Private functions ===========================================================
static void main_helloSequence(void);
static void main_firstCycle(void);
static void main_updateData(void);

// Main function ===============================================================
int main(void)
{
  initHardware_Init();
  main_helloSequence();

  while (1)
    __WFI();

  return 0;
}

// Private functions ===========================================================
static void main_helloSequence(void)
{
  ciaa_initSerialPortMessage();
  ciaa_uart_send2Bash(bash_Red, (uint8_t *)"Aplicacion OFF - Test");

  initHardware_testOutputs();
  ciaa_setPinHigh(&LED_RGB_Rojo);

  ciaa_servo_start();
  ciaa_servo_check(SERVO_CHANNEL_1);
  ciaa_servo_check(SERVO_CHANNEL_2);
  ciaa_servo_check(SERVO_CHANNEL_3);

  ciaa_servo_zeroPosition(SERVO_CHANNEL_3);
  ciaa_servo_stop();


  while( APP_STOPPED == appState)
    __WFI();
}

static void main_firstCycle(void)
{
  ciaa_initSerialPortMessage();
  if(mpu9250_readID() == MPU9250_DEVICE_ID)
  {
    mpu9250_readTemperature_float(pTemperature);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"Temperatura:\n ");
    ciaa_uart_sendData(pTemperature, 1, (UART_DATA_DISPLAY | UART_DATA_FORMAT_SPACE));
    ciaa_uart_putString((uint8_t *)"\n\r", 2);

    ciaa_filter_readAngles(pAngles);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"Posicion inicial:\n ");
    ciaa_uart_sendData(pAngles, 2, (UART_DATA_DISPLAY | UART_DATA_FORMAT_BS));
    ciaa_uart_putString((uint8_t *)"\r\n\n", 3);
  }

  ciaa_uart_send2Bash(bash_Cyan, (uint8_t *)"Pitch\tRoll\r\n");
}

static void main_updateData(void)
{
  if(ciaa_filter_readComplementaryFilter(pAngles) == FILTER_OK)
  {
    ciaa_servo_updatePosition(SERVO_CHANNEL_1, -pAngles[0]);
    ciaa_servo_updatePosition(SERVO_CHANNEL_2, -pAngles[0]);
    ciaa_servo_updatePosition(SERVO_CHANNEL_3,  pAngles[1]);

    ciaa_uart_sendData(pAngles, 2, (UART_DATA_DISPLAY| UART_DATA_FORMAT_TAB));
  }
}

// IQR Handlers ================================================================
void GPIO0_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
  pauseMs(500);

  if(APP_STOPPED == appState)
  {
    appState = APP_RUNNING;
    ciaa_setPinLow(&LED_RGB_Rojo);
    ciaa_setPinHigh(&LED_RGB_Verde);

    if(0 == cycle++)
      main_firstCycle();

    pauseMs(500);

    ciaa_servo_start();
    ciaa_initInterrupt(&MPU_INT_PIN, SCU_MODE_FUNC0);
  }
  else
  {
    appState = APP_STOPPED;
    ciaa_setPinLow(&LED_RGB_Verde);
    ciaa_setPinLow(&LED_Amarillo);
    ciaa_setPinHigh(&LED_RGB_Rojo);

    ciaa_servo_stop();
    ciaa_deInitInterrupt(&MPU_INT_PIN, SCU_MODE_FUNC0);
    pauseMs(500);
  }
}


void GPIO3_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
  main_updateData();
  ciaa_togglePin(&LED_Amarillo);
}
