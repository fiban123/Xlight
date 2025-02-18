#include "../include/stream.hpp"

void AudioStream::init(size_t _frames_per_buffer, size_t _frames_per_fft, PaDeviceIndex _device){
    frames_per_buffer = _frames_per_buffer;
    frames_per_fft = _frames_per_fft;
    input_device = _device;

    PaError err = Pa_Initialize();
    CHECKERR(err);

    PaStreamParameters in_params;
    memset(&in_params, 0, sizeof(in_params));

    const PaDeviceInfo* device_info = Pa_GetDeviceInfo(input_device);

    sample_rate = device_info->defaultSampleRate;
    latency = device_info->defaultLowInputLatency;

    cout << "initializing audio stream with device " << input_device << ", sample rate " << sample_rate << ", latency " << latency << endl;

    in_params.channelCount = 1;
    in_params.device = input_device;
    in_params.sampleFormat = paFloat32;
    in_params.hostApiSpecificStreamInfo = NULL;
    in_params.suggestedLatency = latency;

    err = Pa_OpenStream(
        &stream,
        &in_params,
        NULL,
        sample_rate,
        frames_per_buffer,
        paNoFlag,
        stream_callback,
        this
    );

    CHECKERR(err);

    fft_buffer.resize(frames_per_fft);
    spectrogram.resize(frames_per_fft);

    fft_plan = fftwf_plan_r2r_1d(
        frames_per_fft,
        fft_buffer.data(),
        spectrogram.data(),
        FFTW_R2HC,
        FFTW_PATIENT
    );
}

void AudioStream::start() {
    cout << "starting audio stream of device" << input_device << endl;
    PaError err = Pa_StartStream(stream);
    CHECKERR(err);
    running = true;
}

void AudioStream::stop() {
    cout << "stopping audio stream of device" << input_device << endl;
    PaError err = Pa_StopStream(stream);
    CHECKERR(err);
    running = true;
}

int AudioStream::stream_callback(const void* input_buf, void* output_buf, unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
        void* user_data) {
    
    (void)output_buf;

    AudioStream* instance = static_cast<AudioStream*>(user_data);

    // reformat FFT samples
    copy(instance->fft_buffer.begin() + frames_per_buffer, instance->fft_buffer.end(), instance->fft_buffer.begin());
    
    // add new FFT samples
    copy(input_buf, input_buf + frames_per_buffer, instance->fft_buffer.end() - frames_per_buffer);
    
    // perform FFT
    fftwf_execute(instance->fft_plan);
}

PaDeviceIndex AudioStream::default_input_device() { return Pa_GetDefaultInputDevice(); }

AudioStream::~AudioStream() {
    cout << "closing audio stream of device" << input_device << endl;
    PaError err = Pa_CloseStream(stream);
    CHECKERR(err);

    fftwf_destroy_plan(fft_plan);
}
