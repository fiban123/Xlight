#pragma once

#include <portaudio.h>
#include <fftw3.h>

#include "../include/math.hpp"

#include <stddef.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <functional>
#include <math.h>
#include <map>
#include <deque>

using namespace std;

#define CHECKERR(err) if (err != paNoError){ cout << "PA error: " << Pa_GetErrorText(err) << endl; exit(1);}

//unordered_map<float, deque<float> > volume_factor_map;

struct AudioStream{
    bool initialized = false;
    bool running = false;

    // audio
    float sample_rate;
    float sample_ratio;
    float latency;
    PaDeviceIndex input_device;
    PaStream* stream;
    
    // fft
    size_t frames_per_buffer;
    size_t frames_per_fft;
    fftwf_plan fft_plan;

    float* fft_in_buf;
    float* fft_out_buf;

    vector<float> spectrogram;
    function<void(float)> algo;
    
    bool normalize_spectrogram = true;
    float spectrogram_factor = 1.0f;
    size_t spectrogram_factor_update_rate;
    function<void(float*)> spectrogram_factor_update_func;
    
    static int stream_callback(
        const void* input_buf, void* output_buf, unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
        void* user_data);
    PaDeviceIndex default_input_device();

    void init(size_t _frames_per_buffer, size_t _frames_per_fft, PaDeviceIndex _device,
        size_t _spectrogram_factor_update_rate, function<void(float*)>_spectrogram_factor_update_func);
    void start();
    void stop();


    ~AudioStream();
};

