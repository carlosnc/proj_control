#include "inithardware.h"
#include "edu_ciaa_uart.h"
#include "mpu9250.h"
#include "stdlib.h"

// =============================================================================
int16_t temperature = 0;
int16_t *ptemperature = &temperature;

// Private functions ===========================================================
void setup_sensors(void);
void hello_sequence(void);

// Main function ===============================================================
int main(void)
{
  uint8_t tmp[10];
  uint8_t *ptmp = &tmp[0];

  int16_t accel[3] = {0};
  int16_t *pAccel = &accel[0];

  int16_t gyro[3] = {0};
  int16_t *pGyro = &gyro[0];

  initHardware_Init();

  setup_sensors();
  hello_sequence();

  if(mpu9250_readData_int16(pAccel, pGyro) == MPU9250_OK)
  {
    for(uint8_t i = 0; i < 3; i++)
    {
      itoa(accel[i]%16384, ptmp, 10);
      ciaa_uart_send2Bash(bash_Yellow, ptmp);
      if(i == 2)
        ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\n\r");
      else
        ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\t");
    }

    for(uint8_t i = 0; i < 3; i++)
    {
      itoa(gyro[i]/131, ptmp, 10);
      ciaa_uart_send2Bash(bash_Yellow, ptmp);
      if(i == 2)
        ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\n\r");
      else
        ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\t");
    }
  }

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
    setPinHigh(&LED_RGB_Verde);
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
