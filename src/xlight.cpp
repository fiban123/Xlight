#include "include.hpp"

int main(){
    init_portaudio();

    cout << get_master_volume() << endl;

    init_stream();

    init_algo();

    init_fft();

    init_gui();

    start_stream();
    
    Pa_Sleep(200);

    execute_gui();

    stop_stream();

    end_stream();

    terminate_portaudio();

    terminate_fft();

    return 0;
}