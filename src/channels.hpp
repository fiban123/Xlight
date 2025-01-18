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

unique_ptr<SpectrumAlgo> algo;

void init_algo();