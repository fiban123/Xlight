
// audio & FFT
#define DEFAULT_DEVICE_ID 15
//#define OVEERRIDE_LATENCY x
//#define OVERRIDE_SAMPLE_RATE x

#define FRAMES_PER_BUFFER   512
#define FRAMES_PER_FFT      4096
#define FREQ_LOWER_BOUND    18.0f
#define FREQ_UPPER_BOUND    22000.0f

#define MAGNITUDE_FACTOR 1.00f


// raylib & gui
#define MAX_AMPLITUDE 1000

#define SCREEN_WIDTH    1920
#define SCREEN_HEIGHT   1080

// spectro to RGB
#define N_CHANNELS 4
#define ALGO Algo_SFBGM

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

#define FBGM_BASS_FACTOR                10.0f
#define FBGM_MIDS_FACTOR                30.0f
#define FBGM_HIGHS_FACTOR               60.0f

#define FBGM_MIDS_FLAOT_OVERFLOW_FACTOR 100.0f
#define FBGM_HIGHS_FLOAT_OVERFLOW_FACTOR 30.0f

//SFBGM (spiked frequency bin gradient mapping), a derivative of FBGM

#define SFBGM_BASS_SPIKE_DECAY_FACTOR         20.0f // higher = faster decay. INF = immediate decay, 0 = no decay
#define SFBGM_MIDS_SPIKE_DECAY_FACTOR         18.0f
#define SFBGM_HIGHS_SPIKE_DECAY_FACTOR        17.0f