#include "../include/windows_ifc.hpp"

// this code was copied from the internet !!!
float get_system_volume() {
    float masterVolume = 0.0f;

    // Initialize COM library
    HRESULT hr = CoInitialize(nullptr);
    if (hr < 0) {
        cerr << "Failed to initialize COM library. HRESULT: " << hr << std::endl;
        return masterVolume;
    }

    // Create a device enumerator
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                            __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);
    if (hr < 0) {
        cerr << "Failed to create device enumerator. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return masterVolume;
    }

    // Get the default audio endpoint
    IMMDevice* defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    if (hr < 0) {
        cerr << "Failed to get default audio endpoint. HRESULT: " << hr << std::endl;
        deviceEnumerator->Release();
        CoUninitialize();
        return masterVolume;
    }

    // Get the endpoint volume interface
    IAudioEndpointVolume* endpointVolume = nullptr;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&endpointVolume);
    if (hr < 0) {
        cerr << "Failed to activate endpoint volume. HRESULT: " << hr << std::endl;
        defaultDevice->Release();
        deviceEnumerator->Release();
        CoUninitialize();
        return masterVolume;
    }

    // Get the master volume level (scalar)
    hr = endpointVolume->GetMasterVolumeLevelScalar(&masterVolume);
    if (hr < 0) {
        cerr << "Failed to get master volume level. HRESULT: " << hr << std::endl;
    }

    // Cleanup
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();

    return masterVolume;
}
