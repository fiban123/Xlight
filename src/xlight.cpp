#include "include.hpp"

int main(){
    init_portaudio();

    cout << get_master_volume() << endl;

    init_stream();

    init_fft();

    init_gui();

    init_algo();

    start_stream();
    
    Pa_Sleep(200);

    execute_gui();

    stop_stream();

    end_stream();

    terminate_portaudio();

    terminate_fft();

    return 0;
}