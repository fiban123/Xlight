#include "../include/stream.hpp"

void AudioStream::init(size_t _frames_per_buffer, size_t _frames_per_fft, PaDeviceIndex _device, size_t _spectrogram_factor_update_rate, function<void(float*)> _spectrogram_factor_update_func){
    frames_per_buffer = _frames_per_buffer;
    frames_per_fft = _frames_per_fft;
    input_device = _device;
    spectrogram_factor_update_rate = _spectrogram_factor_update_rate;
    spectrogram_factor_update_func = _spectrogram_factor_update_func;

    PaError err = Pa_Initialize();
    CHECKERR(err);

    PaStreamParameters in_params;
    memset(&in_params, 0, sizeof(in_params));

    const PaDeviceInfo* device_info = Pa_GetDeviceInfo(input_device);

    sample_rate = device_info->defaultSampleRate;
    sample_ratio = sample_rate / (float) frames_per_fft;
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

    initialized = true;

    fft_in_buf.resize(frames_per_fft);
    fft_out_buf.resize(frames_per_fft);
    spectrogram.resize(frames_per_fft / 2);
    spectrogram_derivative.resize(frames_per_fft / 2);

    fft_plan = fftwf_plan_r2r_1d(
        frames_per_fft,
        fft_in_buf.data(),
        fft_out_buf.data(),
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
    running = false;
}

int AudioStream::stream_callback(const void* _input_buf, void* output_buf, unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
        void* user_data) {
    
    (void) output_buf;
    (void) time_info;
    (void) status_flags;

    static size_t count = 0;
    count++;

    AudioStream* instance = static_cast<AudioStream*>(user_data);

    // convert input buffer to float array
    float* input_buf = (float*) _input_buf;

    // if update interval is reached, update the spectrogram factor
    if (count % instance->spectrogram_factor_update_rate == 0){
        instance->spectrogram_factor_update_func(&instance->spectrogram_factor);
        count = 0; // reset counter for next interval
    }

    if (instance->normalize_spectrogram){
        // normalize waveform
        for (size_t i = 0; i < frames_per_buffer; i++){
            input_buf[i] *= instance->spectrogram_factor;
        }
    }


    // reformat FFT samples
    copy(instance->fft_in_buf.begin() + frames_per_buffer, instance->fft_in_buf.end(), instance->fft_in_buf.begin());

    
    // add new FFT samples
    copy(input_buf, input_buf + sizeof(float) * frames_per_buffer, instance->fft_in_buf.end() - frames_per_buffer);
    
    // perform FFT
    fftwf_execute(instance->fft_plan);
    
    // calculate magnitudes
    instance->spectrogram[0] = fabs(instance->fft_out_buf[0]); // 0hz
    
    for (size_t i = 1; i < instance->frames_per_fft / 2 - 1; i++){
        instance->spectrogram[i] = sqrt(
            instance->fft_out_buf[i] * instance->fft_out_buf[i] + instance->fft_out_buf[instance->frames_per_fft - i] * instance->fft_out_buf[instance->frames_per_fft - i]
        );
    }
    
    instance->spectrogram[instance->frames_per_fft / 2 - 1] = fabs(instance->fft_out_buf[instance->frames_per_fft / 2 - 1]); // nyquist frequency

    /*
    float max_waveform = vecmax(instance->fft_in_buf);
    if (volume_factor_map.find(instance->spectrogram_factor) == volume_factor_map.end()){
        volume_factor_map.insert({instance->spectrogram_factor, deque<float>()});
    }
    else{
        volume_factor_map[instance->spectrogram_factor].push_back(0.992537f / max_waveform);
        if (volume_factor_map[instance->spectrogram_factor].size() > 200){
            volume_factor_map[instance->spectrogram_factor].pop_front();
        }
        cout << volume_factor_map[instance->spectrogram_factor].size() << endl;
    }
 
    cout << volume_factor_map.size() << endl;
    */

    // calculate spectrogram derivative
    for (size_t i = 0; i < instance->spectrogram_derivative.size() - 1; i++){
        instance->spectrogram_derivative[i] = instance->spectrogram[i + 1] - instance->spectrogram[i]; 
    }


    return 0;
}

PaDeviceIndex AudioStream::default_input_device() { return Pa_GetDefaultInputDevice(); }

AudioStream::~AudioStream() {
    //CHECKERR(err);
    if (running){
        cout << "closing audio stream of device" << input_device << endl;
        Pa_CloseStream(stream);
    }

    fftwf_destroy_plan(fft_plan);
}
