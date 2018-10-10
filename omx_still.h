#ifndef  OMX_STILL_INC
#define  OMX_STILL_INC

#include <stddef.h>
#include <stdint.h>

#include "error.h"

/******************************************************************************/

struct camera_shot_configuration {
    int32_t shutterSpeed;
    int16_t iso;
    int16_t redGain;
    int16_t blueGain;
    int8_t  quality;
    int8_t  sharpness;
    int8_t  contrast;
    int8_t  brightness;
    int8_t  saturation;
    int8_t  drc;
    int8_t  whiteBalance;
};

/******************************************************************************/

typedef void (*buffer_output_handler)(const uint32_t frame, const uint8_t * const buffer, const size_t length);

WARN_UNUSED enum error_code omx_still_open(struct camera_shot_configuration config);
WARN_UNUSED enum error_code omx_still_close(void);
WARN_UNUSED enum error_code omx_still_shoot(const uint32_t frames, const buffer_output_handler handler);

#endif
