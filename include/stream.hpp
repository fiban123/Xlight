#pragma once

#include <portaudio.h>
#include <fftw3.h>

#include <stddef.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <functional>

using namespace std;

#define CHECKERR(err) if (err != paNoError){ cout << "PA error: " << Pa_GetErrorText(err) << endl; exit(1);}

struct AudioStream{
    bool running;

    // audio
    size_t sample_rate;
    float sample_ratio;
    float latency;
    PaDeviceIndex input_device;
    PaStream* stream;
    
    // fft
    size_t frames_per_buffer;
    size_t frames_per_fft;
    fftwf_plan fft_plan;
    
    vector<float> fft_buffer;
    vector<float> spectrogram;
    function<void(float)> algo;
    
    bool normalize_spectrogram;
    float sysaudio_volume;
    
    
    static int stream_callback(
        const void* input_buf, void* output_buf, unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
        void* user_data);
    PaDeviceIndex default_input_device();

    void init(size_t _frames_per_buffer, size_t _frames_per_fft, PaDeviceIndex _device);
    void start();
    void stop();


    ~AudioStream();
};

