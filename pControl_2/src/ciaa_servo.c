/*
 * ciaa_servo.c
 *
 *  Created on: Mar 2, 2018
 *      Author: carlosnc
 */

// =============================================================================
#include "ciaa_servo.h"

// =============================================================================
static const servo_output_t *servo[3] = {&SERVO_01, &SERVO_02, &SERVO_03};

static volatile uint32_t ticksPerCycle = 0;
static volatile uint32_t posZero = 0;
static volatile uint16_t step = 0;
static volatile int16_t maxAngle = 0;
static volatile int16_t minAngle = 0;

// =============================================================================
static inline void servo_configChannel(const servo_output_t* servo)
{
  uint16_t scu_mode = (SCU_MODE_INACT | SCU_MODE_FUNC1);

  Chip_SCU_PinMuxSet(servo->SCU_Port, servo->SCU_Pin, scu_mode);
  Chip_SCTPWM_SetOutPin(LPC_SCT, servo->SCT_Index, servo->GPIO_Output);
}

// =============================================================================
static uint32_t angle_to_pwm(float32_t angle);

// =============================================================================
void ciaa_servo_init(servo_initStruct_t *servo_initStruct)
{
  uint32_t maxDutyCycle, minDutyCycle;

  maxAngle = servo_initStruct->angle_max;
  minAngle = servo_initStruct->angle_min;

  switch (servo_initStruct->servo_channel)
  {
    case SERVO_CHANNEL_1:
      servo_configChannel(servo[SERVO_CHANNEL_1]);
      break;
    case SERVO_CHANNEL_2:
      servo_configChannel(servo[SERVO_CHANNEL_2]);
      break;
    case SERVO_CHANNEL_3:
      servo_configChannel(servo[SERVO_CHANNEL_3]);
      break;
    default:
      for(uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
        servo_configChannel(servo[i]);
      break;
  }

  Chip_SCTPWM_Init(LPC_SCT);
  Chip_SCTPWM_SetRate(LPC_SCT, 50);
  uint32_t tmp = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 100);

  Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, tmp);
  Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, tmp);
  Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, tmp);

//  maxDutyCycle = 3957600;
//  minDutyCycle = 3590400;

  // 20000 --- 100
  //   600 --- x = 3  --> 100 - 3 = 97%
  // 20000 --- 100
  //  2400 --- x = 12 --> 100 - 12 = 88%
  maxDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 97);
  minDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 88);

  ticksPerCycle = (maxDutyCycle - minDutyCycle);

  step = ticksPerCycle/180;
  posZero = maxDutyCycle - (servo_initStruct->pos_zero * step);
}

void ciaa_servo_check(servo_channel_t servo_channel)
{
  uint8_t tmpStep = (maxAngle - minAngle)/4;
  float32_t tmpAngle = (float32_t)minAngle;

  ciaa_servo_zeroPosition(servo_channel);

  if(servo_channel == SERVO_CHANNEL_ALL)
  {
    for(uint8_t i = 0; i < 5; i++)
    {
      ciaa_servo_updatePosition((servo_channel_t)i, tmpAngle);
      tmpAngle += tmpStep;
      pauseMs(1000);
    }
  }
  else
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      ciaa_servo_updatePosition(servo_channel, tmpAngle);
      tmpAngle += tmpStep;
      pauseMs(1000);
    }
  }

  ciaa_servo_zeroPosition(servo_channel);
}

void ciaa_servo_zeroPosition(servo_channel_t servo_channel)
{
  if(servo_channel == SERVO_CHANNEL_ALL)
  {
    for(uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
      ciaa_servo_updatePosition((servo_channel_t)i, 0.0f);
  }
  else
  {
    ciaa_servo_updatePosition(servo_channel, 0.0f);
  }
}

void ciaa_servo_updatePosition(servo_channel_t servo_channel, float32_t angle)
{
  uint32_t servo_pos = 0;

  servo_pos = angle_to_pwm(angle);

  switch (servo_channel)
  {
    case SERVO_CHANNEL_1:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_1]->SCT_Index, servo_pos);
      break;
    case SERVO_CHANNEL_2:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_2]->SCT_Index, servo_pos);
      break;
    case SERVO_CHANNEL_3:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_3]->SCT_Index, servo_pos);
      break;
    default:
      for (uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
        Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[i]->SCT_Index, servo_pos);
      break;
  }
}

// =============================================================================
static uint32_t angle_to_pwm(float32_t angle)
{
  int16_t int_degree = 0;
  uint32_t pwm_pos = 0;

  if (angle < 0)
    int_degree = ((int16_t) angle - 0.5f);

  if (angle > 0)
    int_degree = ((int16_t) angle + 0.5f);

  (int_degree > maxAngle) ? (int_degree = maxAngle) : (int_degree);
  (int_degree < minAngle) ? (int_degree = minAngle) : (int_degree);

  pwm_pos = ((uint32_t) posZero - (step * int_degree));

  return pwm_pos;
}
