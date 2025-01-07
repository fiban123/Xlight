
void init_fft(){ // initialize the FFT properties & plan
    sample_ratio = (float)(sample_rate / (double) FRAMES_PER_FFT);

    start_bin_index = get_bin(FREQ_LOWER_BOUND);
    end_bin_index = get_bin(FREQ_UPPER_BOUND);

    n_bins = end_bin_index - start_bin_index;

    normalized_magnitudes = vector<float>(n_bins);

    plan = fftwf_plan_r2r_1d(
        FRAMES_PER_FFT,
        fft_samples.data(),
        spectrogram.data(),
        FFTW_R2HC,
        FFTW_PATIENT
    );
}

void terminate_fft() { // frees memory allocated to FFT plan
    fftwf_destroy_plan(plan);
}

// code not made by me. returns the master volume. have no idea how it works but it works and this is the easiest way to do it.
float get_master_volume(){
    float masterVolume = 0.0f;

    // Initialize COM library
    windows::HRESULT hr = windows::CoInitialize(nullptr);
    if (hr < 0) {
        std::cerr << "Failed to initialize COM library. HRESULT: " << hr << std::endl;
        return masterVolume;
    }

    // Create a device enumerator
    windows::IMMDeviceEnumerator* deviceEnumerator = nullptr;
    hr = windows::CoCreateInstance(windows::__uuidof(windows::MMDeviceEnumerator), nullptr, windows::CLSCTX_INPROC_SERVER,
                            windows::__uuidof(windows::IMMDeviceEnumerator), (void**)&deviceEnumerator);
    if (hr < 0) {
        std::cerr << "Failed to create device enumerator. HRESULT: " << hr << std::endl;
        windows::CoUninitialize();
        return masterVolume;
    }

    // Get the default audio endpoint
    windows::IMMDevice* defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(windows::eRender, windows::eConsole, &defaultDevice);
    if (hr < 0) {
        std::cerr << "Failed to get default audio endpoint. HRESULT: " << hr << std::endl;
        deviceEnumerator->Release();
        windows::CoUninitialize();
        return masterVolume;
    }

    // Get the endpoint volume interface
    windows::IAudioEndpointVolume* endpointVolume = nullptr;
    hr = defaultDevice->Activate(windows::__uuidof(windows::IAudioEndpointVolume), windows::CLSCTX_INPROC_SERVER, nullptr, (void**)&endpointVolume);
    if (hr < 0) {
        std::cerr << "Failed to activate endpoint volume. HRESULT: " << hr << std::endl;
        defaultDevice->Release();
        deviceEnumerator->Release();
        windows::CoUninitialize();
        return masterVolume;
    }

    // Get the master volume level (scalar)
    hr = endpointVolume->GetMasterVolumeLevelScalar(&masterVolume);
    if (hr < 0) {
        std::cerr << "Failed to get master volume level. HRESULT: " << hr << std::endl;
    }

    // Cleanup
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    windows::CoUninitialize();

    return masterVolume;
}

static int callback( // the audio callback function
    const void* input_buf, void* output_buf, unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
    void* user_data) {

    (void) output_buf; // no-op

    float *in = (float*)input_buf;

    // reformat FFT samples to allow for new FFT samples
    memmove(fft_samples.data(), fft_samples.data() + FRAMES_PER_BUFFER, (FRAMES_PER_FFT - FRAMES_PER_BUFFER) * sizeof(float));

    // copy new samples into FFT samples;
    memcpy(fft_samples.data() + (FRAMES_PER_FFT - FRAMES_PER_BUFFER), in, FRAMES_PER_BUFFER * sizeof(float));

    // execute the FFT
    fftwf_execute(plan);

    // calculate normalized magnitudes
    for (unsigned int i = start_bin_index; i < end_bin_index; i++){
        float magnitude;
        if (i == 0 || i == FRAMES_PER_FFT / 2){ // nyquist frequency or 0hz frequency have no complex part.
            magnitude = fabs(spectrogram[i]);
        }
        else{
            magnitude = sqrt(spectrogram[i] * spectrogram[i] + spectrogram[FRAMES_PER_FFT - i] * spectrogram[FRAMES_PER_FFT - i]);
        }

        magnitude *= (1.0f / linear_master_volume) * MAGNITUDE_FACTOR;

        normalized_magnitudes[i - start_bin_index] = magnitude;
    }

    algo->execute(&base_channels, &transformed_channels, &normalized_magnitudes);

    // get master volume every 10 frames
    if (frame_count % 10 == 0){
        // windows volume isnt linear :(, so we have to normalize it.
        linear_master_volume = pow(get_master_volume(), 1.7f);
    }
    frame_count++;

    return 0;
} 