PaError err;
PaStream* stream;

void init_portaudio();

void init_stream();

void start_stream();

void stop_stream();

void end_stream();

void terminate_portaudio();

void print_device_info(int i);