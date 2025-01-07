struct CColor{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    inline void shift_hue(float hue);

    CColor(uint8_t _r, uint8_t _g, uint8_t _b);
};

struct Channel{
    //float prev_amp = 0.0f;
    CColor col = CColor(0, 0, 0);
};

// structs for easy algorithm changes

array<Channel, N_CHANNELS> base_channels;
array<Channel, N_CHANNELS> transformed_channels;

// contains channel algorithms and transform algorithms that convert spectrogram data into RGB values.
class SpectrumAlgo{ // abstract class
    public:
        virtual void base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes) = 0; 

        virtual void transform_algo(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes) = 0;

        virtual void init() = 0;

        void execute(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes);
};

struct Algo_FBGM : public SpectrumAlgo{
    public:
        virtual void base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes);

        virtual void transform_algo(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes){};

        virtual void init(){};
};

struct Algo_SFBGM : public SpectrumAlgo{
    public:
        float prev_bass_base_brightness = 0.0f; // previous base brightness value
        float bass_spiked_brightness = 0.0f; // affected by rate of positive change in base brightness and spike decay value

        float prev_mids_base_brightness = 0.0f;
        float mids_spiked_brightness = 0.0f;

        float prev_highs_base_brightness = 0.0f;
        float highs_spiked_brightness = 0.0f;

        virtual void base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes);

        virtual void transform_algo(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes){};

        virtual void init(){};
};

unique_ptr<SpectrumAlgo> algo;

void init_algo();