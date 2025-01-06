void print_device_info(int i){ // prints info about an audio device
    const PaDeviceInfo* info;
    info = Pa_GetDeviceInfo(i);
    cout << "device " << i << endl;
    cout << "  name " << info->name << endl;
    cout << "  max input channels " << info->maxInputChannels << endl;
    cout << "  max output channels " << info->maxOutputChannels << endl;
    cout << "  default sample rate " << info->defaultSampleRate << endl;
    cout << "  default input low latency " << info->defaultLowInputLatency << endl;
    cout << "  default input high latency " << info->defaultHighInputLatency << endl;
    cout << "  default output low latency " << info->defaultLowOutputLatency << endl;
    cout << "  default output high latency " << info->defaultHighOutputLatency << endl;
}

void init_portaudio() { // initialize portaudio
    err = Pa_Initialize();
    CHECKERR(err);

    int num_devices = Pa_GetDeviceCount();

    CHECKSIGN(num_devices);
    CHECKZERO(num_devices);

    //for (int i = 0; i < num_devices; i++){
        //print_device_info(i);
    //}

    print_device_info(DEVICE_ID);
}

void init_stream() { // inizalize stream & stream properties
    const PaDeviceInfo* info;

    info = Pa_GetDeviceInfo(DEVICE_ID);

    PaStreamParameters in_params;

    memset(&in_params, 0, sizeof(in_params));

    in_params.channelCount = 1;
    in_params.device = DEVICE_ID;
    in_params.hostApiSpecificStreamInfo = NULL;
    in_params.sampleFormat = paFloat32;
    in_params.suggestedLatency = info->defaultLowInputLatency;

    err = Pa_OpenStream(
        &stream,
        &in_params,
        NULL,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        callback,
        NULL
    );
    CHECKERR(err);
}

void start_stream() { // start audio stream
    err = Pa_StartStream(stream);
    CHECKERR(err);
}

void stop_stream() { // stop audio stream
    err = Pa_StopStream(stream);
    CHECKERR(err);
}

void end_stream() { // end & terminate audio stream
    err = Pa_CloseStream(stream);;
    CHECKERR(err);
}

void terminate_portaudio() { // terminate portaudio library
    err = Pa_Terminate();
    CHECKERR(err);
}
