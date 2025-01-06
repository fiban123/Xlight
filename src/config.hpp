
// audio & FFT
#define DEVICE_ID 15

#define FRAMES_PER_BUFFER   256
#define FRAMES_PER_FFT      1024
#define SAMPLE_RATE         44100.0f
#define FREQ_LOWER_BOUND    18.0f
#define FREQ_UPPER_BOUND    22000.0f

#define MAGNITUDE_FACTOR 4.44f
// raylib & gui
#define MAX_AMPLITUDE 1000


#define SCREEN_WIDTH    1920
#define SCREEN_HEIGHT   1080

// spectro to RGB
#define N_CHANNELS 4
#define ALGO Algo_FBGM

// algos

// FBGM (frequency bin gradient mapping)

// HD = Hue Determination
#define FBGM_BASS_START                 FREQ_LOWER_BOUND
#define FBGM_BASS_G2_START              150.0f
#define FBGM_BASS_G2_END                250.0f


#define FBGM_MIDS_G1_START              250.0f
#define FBGM_MIDS_G1_END                350.0f
#define FBGM_MIDS_G2_START              4250.0f
#define FBGM_MIDS_G2_END                4750.0f

#define FBGM_MIDS_HD_G1_START           250.0f
#define FBGM_MIDS_HD_G1_END             300.0f
#define FBGM_MIDS_HD_END                4500.0f


#define FBGM_HIGHS_G1_START             4250.0f
#define FBGM_HIGHS_G1_END               4750.0f
#define FBGM_HIGHS_END                  FREQ_UPPER_BOUND

#define FBGM_HIGHS_HD_START             4500.0f
#define FBGM_HIGHS_HD_END               FREQ_UPPER_BOUND