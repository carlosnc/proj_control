#include "inithardware.h"
#include "edu_ciaa_uart.h"
#include "mpu9250.h"
#include "stdlib.h"

// =============================================================================
int16_t temperature = 0;
int16_t *ptemperature = &temperature;

int16_t aAccel[3] = { 0 };
int16_t *pAccel = &aAccel[0];

int16_t aGyro[3] = { 0 };
int16_t *pGyro = &aGyro[0];

// Private functions ===========================================================
void setup_sensors(void);
void hello_sequence(void);
void update_data(void);

// Main function ===============================================================
int main(void)
{
  initHardware_Init();
  hello_sequence();
  setup_sensors();

  pauseMs(5000);
  ciaa_initInterrupt(&MPU_INT_PIN, SCU_MODE_FUNC0);

  while (1)
    __WFI();

  return 0;
}

// Private functions ===========================================================
void setup_sensors(void)
{
  mpu9250_InitStruct mpu9250_InitStruct;
  mpu9250_InitStruct.Interface = MPU9250_INTERFACE_I2C;
  mpu9250_InitStruct.SampleRate = 4;

  mpu9250_InitStruct.Accel_Axes = MPU9250_ACCEL_XYZ_ENABLE;
  mpu9250_InitStruct.Accel_Scale = MPU9250_ACCEL_FULLSCALE_2G;
  mpu9250_InitStruct.Accel_LPF = MPU9250_ACCEL_LPF_44_8HZ;

  mpu9250_InitStruct.Gyro_Axes = MPU9250_GYRO_XYZ_ENABLE;
  mpu9250_InitStruct.Gyro_Scale = MPU9250_GYRO_FULLSCALE_250DPS;
  mpu9250_InitStruct.Gyro_LPF = MPU9250_GYRO_LPF_41HZ;

  if(mpu9250_init(&mpu9250_InitStruct) == MPU9250_OK)
    ciaa_setPinHigh(&LED_RGB_Verde);
}

void hello_sequence(void)
{
  uint8_t tmp[10];
  uint8_t *ptmp = &tmp[0];
  initHardware_testOutputs();

  ciaa_uart_send2Bash(bash_cursor2Home, (uint8_t*) "\r");
  ciaa_uart_send2Bash(bash_ClearScreen, (uint8_t*) "\r");
  ciaa_uart_send2Bash(bash_Green, (uint8_t*) "\t\t..:: Carlos.n.Contrera");
  ciaa_uart_send2Bash(bash_Normal, (uint8_t*) " - ");
  ciaa_uart_send2Bash(bash_Blue, (uint8_t*) "Teoria del control 2 ::..\n\n\r");

  if(mpu9250_readID() == MPU9250_DEVICE_ID)
  {
    mpu9250_readTemperature_int16(ptemperature);
    itoa((temperature/100), ptmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t*) "MPU9250 en linea...");
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t*) "\tTemperatura: ");
    ciaa_uart_send2Bash(bash_Yellow, ptmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)" C\n\n\r");
  }

}

void update_data(void)
{
  uint8_t tmp[10];

  if(mpu9250_readAccelData_int16(pAccel) == MPU9250_OK)
  {
    for (uint8_t i = 0; i < 3; i++)
    {
      itoa(aAccel[i], &tmp, 10);
      ciaa_uart_send2Bash(bash_Cyan, &tmp);

      if(i == 2)
        ciaa_uart_putString((uint8_t *)"\n\r", 2);
      else
        ciaa_uart_putString((uint8_t *)"\t", 1);
    }
  }
}

// =============================================================================
void GPIO3_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
  ciaa_togglePin(&LED_Amarillo);
  update_data();
}
