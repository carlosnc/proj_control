#include "inithardware.h"
#include "stdlib.h"

// Global variables ============================================================
const uint8_t sSubjectMsg[30] = "\t\t ..:: Teoria del Control 2";
const uint8_t sAuthorMsg[25] = "Contrera Carlos ::..\n\n\r";

volatile ciaa_app_state_t appState = APP_STOPPED;

int16_t temperature = 0x0000;
int16_t *pTemperature = &temperature;

// Private functions ===========================================================
static void hello_sequence(void);

// Main function ===============================================================
int main(void)
{
  initHardware_Init();

  while (1)
    __WFI();

  return 0;
}

// Private functions ===========================================================
static void hello_sequence(void)
{
  __NOP();
}

// IQR Handlers ================================================================
void GPIO0_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
  pauseMs(250);

  if(APP_STOPPED == appState)
  {
    appState = APP_RUNNING;
    ciaa_setPinLow(&LED_RGB_Rojo);
    ciaa_setPinHigh(&LED_RGB_Verde);
  }
  else
  {
    appState = APP_STOPPED;
    ciaa_setPinLow(&LED_RGB_Verde);
    ciaa_setPinHigh(&LED_RGB_Rojo);
  }
}

void GPIO3_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
  ciaa_togglePin(&LED_Amarillo);
}
