#pragma once
#include <stdint.h>
#include <stdlib.h>

/*
  sample format is 8 bit unsigned [0 .. 255]
 */
#define SAMPLE_RATE_HZ 16000U
extern const size_t   samples_len;    // see audio_sample.c for value
extern const uint8_t  samples_raw[];  // see audio_sample.c for values
