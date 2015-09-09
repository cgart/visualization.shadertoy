#ifndef AUDIO_HPP__
#define AUDIO_HPP__

#include "base.hpp"
#include "kiss_fft.h"

#define SMOOTHING_TIME_CONSTANT (0.8)
#define MIN_DECIBELS (-100.0)
#define MAX_DECIBELS (-30.0)

#define AUDIO_BUFFER (1024)
#define NUM_BANDS (AUDIO_BUFFER / 2)


namespace audio
{

void Init();
void WriteToBuffer(const float *input, size_t length, size_t channels);
void NewAudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength);
void Destroy();
GLubyte* AudioData();
int GetSamplesPerSec();
void SetSamplesPerSec(int samples);

}

#endif 
