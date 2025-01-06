PaError err;
PaStream* stream;

unsigned int device;
unsigned int sample_rate;
float latency;
bool debug_mode;


void init_portaudio();

void init_stream();

void start_stream();

void stop_stream();

void end_stream();

void terminate_portaudio();

void print_device_info(int i);