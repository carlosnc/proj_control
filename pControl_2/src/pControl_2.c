#include "inithardware.h"
#include "edu_ciaa_uart.h"
#include "mpu9250.h"
#include "ciaa_servo.h"
#include "ciaa_estimador.h"
#include "stdlib.h"

// =============================================================================
int16_t temperature = 0;
int16_t *ptemperature = &temperature;

int16_t aAccel[3] = { 0 };
int16_t *pAccel = &aAccel[0];

int16_t aGyro[3] = { 0 };
int16_t *pGyro = &aGyro[0];

float32_t aAngles[2] = { 0.0f };
float32_t *pAngles = &aAngles[0];

// Private functions ===========================================================
void setup_sensors(void);
void hello_sequence(void);
void update_data(void);

// Main function ===============================================================
int main(void)
{
  int16_t tmp[10];

  initHardware_Init();
  hello_sequence();
  setup_sensors();

  pauseMs(1000);

  if(ciaa_filter_readAngles(pAngles) == FILTER_OK)
  {
    itoa(((int32_t) aAngles[0]/1), &tmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, &tmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\t");
    itoa(((int32_t) aAngles[1]/1), &tmp, 10);
    ciaa_uart_send2Bash(bash_Yellow, &tmp);
    ciaa_uart_send2Bash(bash_Yellow, (uint8_t *)"\n\r");
  }

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

  servo_init_t servo_initStruct;
  servo_initStruct.pos_zero = 90;
  servo_initStruct.angle_min = -60;
  servo_initStruct.angle_max = 60;
  servo_initStruct.servo_channel = SERVO_CHANNEL_ALL;
  ciaa_servo_init(&servo_initStruct);

  filter_init_t filter_initStruct;
  filter_initStruct.freq_update = 80;
  filter_initStruct.weight_of_filter = 0.10f;
  ciaa_filter_init(&filter_initStruct);
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
  ciaa_togglePin(&LED_Verde);
}

// =============================================================================
void GPIO3_IRQHandler(void)
{
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
  ciaa_togglePin(&LED_Amarillo);
  update_data();
}
