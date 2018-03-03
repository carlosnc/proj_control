#ifndef CIAA_ESTIMADOR_H_
#define CIAA_ESTIMADOR_H_

// =============================================================================
#include "chip.h"
#include "mpu9250.h"

// =============================================================================
typedef float float32_t;

// =============================================================================
typedef enum
{
  FILTER_ERROR = 0,
  FILTER_OK,
} filter_status_t;

typedef struct
{
    int16_t freq_update;
    float32_t weight_of_filter;
} filter_initStruct_t;

// =============================================================================
void ciaa_filter_init(filter_initStruct_t *filter_InitStruct);
filter_status_t ciaa_filter_readAngles(float32_t *pAngles);
filter_status_t ciaa_filter_readComplementaryFilter( float32_t* pFilterOutput );

// =============================================================================

#endif /* CIAA_ESTIMADOR_H_ */
