array<float, FRAMES_PER_FFT> spectrogram; // output of FFT
array<float, FRAMES_PER_FFT> fft_samples; // samples used by FFT

vector<float> normalized_magnitudes; // normalized magnitudes of FFT operation

unsigned int end_bin_index;
unsigned int start_bin_index;
unsigned int n_bins;

fftwf_plan plan;

float sample_ratio;

float linear_master_volume;

size_t frame_count;

float get_master_volume();

void init_fft();

void terminate_fft();

static int callback(
    const void* input_buf, void* output_buf, unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
    void* user_data);
