#include "inithardware.h"
#include "stdlib.h"

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types";

// Global variables ============================================================
volatile ciaa_app_state_t appState = APP_STOPPED;
volatile uint8_t cycle = 0;

int16_t temperature = 0x0000;
int16_t *pTemperature = &temperature;

// aAngles[0]: Pitch angle
// aAngles[1]: Roll  angle
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
  initHardware_testOutputs();
  main_helloSequence();

  while (1)
    __WFI();

  return 0;
}

// Private functions ===========================================================
static void main_helloSequence(void)
{
  ciaa_setPinHigh(&LED_RGB_Rojo);
  ciaa_initSerialPortMessage();
  ciaa_uart_send2Bash(bash_Red, (uint8_t *)"Aplicacion OFF");

  while( APP_STOPPED == appState)
    __WFI();
}

static void main_firstCycle(void)
{
  uint8_t sTmp[10];

  ciaa_initSerialPortMessage();
  if(mpu9250_readID() == MPU9250_DEVICE_ID)
  {
    mpu9250_readTemperature_int16(pTemperature);

    itoa((temperature/100), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"Temperatura:\t\t ");
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    itoa((temperature%100), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)".");
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"C\n\r");

    ciaa_filter_readAngles(pAngles);
    itoa(((int32_t)aAngles[0]/1), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"Posicion Inicial:\t ");
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    itoa(((int32_t)aAngles[0]%1), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)".");
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"/");

    itoa(((int32_t)aAngles[1]/1), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    itoa(((int32_t)aAngles[1]%1), &sTmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)".");
    ciaa_uart_send2Bash(bash_Yellow, sTmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\n\r");
  }
}

static void main_updateData(void)
{
  // ToDo: implement.
  __NOP();
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
    ciaa_initInterrupt(&MPU_INT_PIN, SCU_MODE_FUNC0);
  }
  else
  {
    appState = APP_STOPPED;
    ciaa_setPinLow(&LED_RGB_Verde);
    ciaa_setPinLow(&LED_Amarillo);
    ciaa_setPinHigh(&LED_RGB_Rojo);
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
