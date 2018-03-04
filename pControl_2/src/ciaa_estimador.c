// =============================================================================
#include "ciaa_estimador.h"
#include "math.h"

// =============================================================================
static const float32_t PI = 3.14159265358979f;

volatile float32_t dt = 0.0f;
volatile float32_t weight = 0.0f;
volatile float32_t past_gyro[2] = { 0.0f };
volatile float32_t previous_pitch[2] = { 0.0f };
volatile float32_t previous_roll[2] = { 0.0f };

// =============================================================================
static inline float32_t RAD2DEG(float32_t x)
{
  return ((float32_t) (x*(180.0f/PI)));
}

static inline void sumPower_float32_t(float32_t *pSource, uint8_t len, float32_t *pRes)
{
  for(uint8_t i = 0; i < len; i++)
    *pRes += (pSource[i]*pSource[i]);
}

// =============================================================================
static float32_t efast_inverse_sqrt(float32_t x);
static void ecalculate_Angle(float32_t *pAccelData, float32_t *pGyroData, float32_t *pRes);
static void enormalize_measure(float32_t *aData);
static void ecomplementary_filter(float32_t *Data, float32_t *gData, float32_t *filtered_angle);

// =============================================================================
void ciaa_filter_init(filter_initStruct_t *filter_InitStruct)
{
  dt = ((float32_t) 1 / filter_InitStruct->freq_update);
  weight = filter_InitStruct->weight_of_filter;
}

filter_status_t ciaa_filter_readAngles(float32_t *pAngles)
{
  filter_status_t status = FILTER_ERROR;

  float32_t aAngBuff[4] = { 0.0f };
  float32_t aAccelData[3] = { 0.0f };
  float32_t aGyroData[3] = { 0.0f };

  float32_t* pAngBuff = &aAngBuff[0];
  float32_t* pAccelData = &aAccelData[0];
  float32_t* pGyroData = &aGyroData[0];

  if( mpu9250_readData_float(pAccelData, pGyroData) == MPU9250_OK )
    status = FILTER_OK;

  ecalculate_Angle(pAccelData, pGyroData, pAngBuff);

  for(uint8_t i = 0; i < 2; i++)
    pAngles[i] = pAngBuff[2 + i] + RAD2DEG(pAngBuff[i]);

  return status;
}

filter_status_t ciaa_filter_readComplementaryFilter( float32_t* pFilterOutput )
{
  filter_status_t status = FILTER_ERROR;
  /* aData[3]: Ax, Ay, Az
   * gData[3]: Gx, Gy, Gz
   * aAngBuff[4]: acell_Pitch, accel_Roll, gyro_Pitch, gyro_Roll
   * past_gyro[2]: past_pitch, past_roll
   */

  float32_t aAngBuff[4] = { 0.0f };
  float32_t aAccelData[3] = { 0.0f };
  float32_t aGyroData[3] = { 0.0f };
  float32_t aResult[2] = { 0.0f };

  float32_t* pAngBuff = &aAngBuff[0];
  float32_t* pAccelData = &aAccelData[0];
  float32_t* pGyroData = &aGyroData[0];
  float32_t* pResult = &aResult[0];

  if( mpu9250_readData_float(pAccelData, pGyroData) == MPU9250_OK )
    status = FILTER_OK;

  ecalculate_Angle(pAccelData, pGyroData, pAngBuff);
  ecomplementary_filter(pAngBuff, pGyroData, pResult);

  for (uint8_t i = 0; i < 2; i++)
    pFilterOutput[i] = pResult[i];

  return status;
}

// =============================================================================
static float32_t efast_inverse_sqrt(float32_t x)
{
  float32_t xhalf = 0.5f * x;
  int32_t i = *(int32_t *) &x;

  i = 0x5F3759DF - (i >> 1);    // Magic number!!! RTFpaper.
  x = *(float32_t *) &i;

  x = x * (1.5f - (xhalf * x * x));
  x = x * (1.5f - (xhalf * x * x));
  x = x * (1.5f - (xhalf * x * x));

  return x;
}

static void ecalculate_Angle( float32_t *pAccelData, float32_t *pGyroData, float32_t *pRes )
{
  float32_t tmp = 0.0f;

  enormalize_measure(pAccelData);

  tmp += powf(pAccelData[1], 2);
  tmp += powf(pAccelData[2], 2);
  pRes[0] = atan2f(-pAccelData[0], sqrtf(tmp));

  tmp = 0.0f;
  tmp += powf(pAccelData[0], 2);
  tmp += powf(pAccelData[2], 2);
  pRes[1] = atan2f(-pAccelData[1], sqrtf(tmp));

  pRes[2] = past_gyro[0] + pGyroData[0] * dt;
  pRes[3] = past_gyro[1] + pGyroData[1] * dt;

  past_gyro[0] = pRes[2];
  past_gyro[1] = pRes[3];
}

static void enormalize_measure( float32_t *aData )
{
  float32_t tmp = 0.0f;
  float32_t *ptmp = &tmp;
  float32_t norm_g_vector = 0.0f;

  sumPower_float32_t(aData, 3, ptmp);
  norm_g_vector = efast_inverse_sqrt(tmp);

  for (uint8_t i = 0; i < 3; i++)
    aData[i] /= norm_g_vector;
}

/*
 * @out filtered_angle[0] = pitch angle
 * @out filtered_angle[1] = roll angle
 */
static void ecomplementary_filter(float32_t *Data, float32_t *gData, float32_t *filtered_angle)
{
  float32_t aPitch_Est, aRoll_Est;
  float32_t gPitch_Est, gRoll_Est;

  aPitch_Est = weight * previous_pitch[0] + (1 - weight) * Data[0];
  aRoll_Est = weight * previous_roll[0] + (1 - weight) * Data[1];

  gPitch_Est = weight * previous_pitch[1] + gData[1] * dt;
  gRoll_Est = weight * previous_roll[1] + gData[0] * dt;

  previous_pitch[0] = aPitch_Est;
  previous_pitch[1] = gPitch_Est;

  previous_roll[0] = aRoll_Est;
  previous_roll[1] = gRoll_Est;

  filtered_angle[0] = gPitch_Est + RAD2DEG(aPitch_Est);
  filtered_angle[1] = gRoll_Est + RAD2DEG(aRoll_Est);
}
