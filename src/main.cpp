#include "../src/xlight.cpp"

#include <portaudio.h>

#include <map>

//#define PC

#ifdef PC
unordered_map<unsigned short int, float> volume_factor_table = {
    {0, 0.0f}, 
    {1, 348.017f}, {2, 162.759f}, {3, 97.7864f}, {4, 63.6195f}, {5, 45.0988f},
    {6, 34.7501f}, {7, 26.8908f}, {8, 20.6439f}, {9, 17.4077f}, {10, 14.6139f},
    {11, 12.3246f}, {12, 11.3042f}, {13, 9.51737f}, {14, 8.70877f}, {15, 7.31027f},
    {16, 6.73293f}, {17, 6.16599f}, {18, 5.65525f}, {19, 5.16973f}, {20, 4.76116f},
    {21, 4.35627f}, {22, 3.99875f}, {23, 3.65646f}, {24, 3.36768f}, {25, 3.0839f},
    {26, 2.82814f}, {27, 2.58551f}, {28, 2.38115f}, {29, 2.38112f}, {30, 2.1785f},
    {31, 1.99991f}, {32, 1.99992f}, {33, 1.82847f}, {34, 1.82849f}, {35, 1.68413f},
    {36, 1.68414f}, {37, 1.54214f}, {38, 1.41436f}, {39, 1.41437f}, {40, 1.29294f},
    {41, 1.29291f}, {42, 1.29292f}, {43, 1.19069f}, {44, 1.19071f}, {45, 1.08935f},
    {46, 1.08936f}, {47, 1.00002f}, {48, 1.00001f}, {49, 1.00001f}, {50, 1.0f}
};

void spectrogram_factor_update_func(float* value){
    float sysvolume = get_system_volume();
    unsigned short int rounded_sysvolume = (unsigned short int)(50.0f * sysvolume);
    *value = volume_factor_table.at(rounded_sysvolume);
}
#else
void spectrogram_factor_update_func(float* value){
    *value = 1;
}
#endif
int main(){
    Pa_Initialize();

    Xlight xlight(false);

    //xlight.gui.stream.init(128, 2048, xlight.gui.stream.default_input_device(), 10, spectrogram_factor_update_func); 

    xlight.gui.init(240, sf::VideoMode::getDesktopMode(), 1000, 128, 2048, xlight.gui.stream.default_input_device(), 10, spectrogram_factor_update_func, 
    ALGO_FBGM);

    xlight.dmx.channels = &xlight.gui.channels;
    
    xlight.interface.init(xlight.gui.algo.get());


    xlight.gui.stream.start();

    xlight.gui.start();
    //xlight.dmx.start();
    xlight.interface.start();

    xlight.gui.wait_for_exit();
    //xlight.dmx.stop();

    return 0;
}