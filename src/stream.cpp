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

    cout << num_devices << endl;

    debug_mode = false;
    if (debug_mode){
        device = DEVICE_ID;
        sample_rate = SAMPLE_RATE;
    }
    else{
        // list all devices, along with the default
        for (int i = 0; i < num_devices; i++){
            print_device_info(i);
        }
        cout << "default input device: " << Pa_GetDefaultInputDevice() << endl;
        cout << "select a device" << endl;
        string device_str;

        cin >> device_str;

        device = stoi(device_str);
    }

    CHECKSIGN(num_devices);
    CHECKZERO(num_devices);

    print_device_info(device);
}

void init_stream() { // inizalize stream & stream properties
    const PaDeviceInfo* info;

    info = Pa_GetDeviceInfo(device);

    PaStreamParameters in_params;

    memset(&in_params, 0, sizeof(in_params));

    in_params.channelCount = 1;
    in_params.device = device;
    in_params.hostApiSpecificStreamInfo = NULL;
    in_params.sampleFormat = paFloat32;
    in_params.suggestedLatency = info->defaultLowInputLatency;

    sample_rate = info->defaultSampleRate;
    latency = info->defaultLowInputLatency;


    if (debug_mode){
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
    }
    else{
        err = Pa_OpenStream(
            &stream,
            &in_params,
            NULL,
            sample_rate,
            FRAMES_PER_BUFFER,
            paClipOff,
            callback,
            NULL
        );
    }

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
