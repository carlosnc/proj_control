#ifndef CIAA_SERVO_H_
#define CIAA_SERVO_H_

#include "chip.h"
#include "inithardware.h"

// =============================================================================
typedef float float32_t;

typedef enum
{
  SERVO_CHANNEL_1 = 0,
  SERVO_CHANNEL_2,
  SERVO_CHANNEL_3,
  SERVO_CHANNEL_ALL,
} servo_channel_t;

typedef struct
{
    uint32_t pos_zero;
     int16_t angle_min;
     int16_t angle_max;
     servo_channel_t servo_channel;
} servo_init_t;

typedef struct
{
    uint8_t SCU_Port;
    uint8_t SCU_Pin;
    uint8_t SCT_Index;
    uint8_t GPIO_Output;
} servo_output_t;

// =============================================================================
static const servo_output_t SERVO_01 = {4, 2, 1, 0};
static const servo_output_t SERVO_02 = {4, 1, 2, 1};
static const servo_output_t SERVO_03 = {4, 3, 3, 3};

// =============================================================================
void ciaa_servo_init(servo_init_t *servo_initStruct);
void ciaa_servo_check(servo_channel_t servo_channel);
void ciaa_servo_zeroPosition(servo_channel_t servo_channel);
void ciaa_servo_updatePosition(servo_channel_t servo_channel, float32_t angle);

// =============================================================================
static inline void ciaa_servo_start(void)
{
  Chip_SCTPWM_Start(LPC_SCT);
}

// =============================================================================
#endif /* CIAA_SERVO_H_ */
