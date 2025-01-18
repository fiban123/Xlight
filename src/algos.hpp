constexpr inline float smoothstep(float edge0, float edge1, float x);

constexpr inline float smoothstep2(float edge0_0, float edge1_0, float edge0_1, float edge1_1, float x);

const inline unsigned int get_magnitude_bin(float freq);

constexpr inline float get_spiked_brightness(float spiked_brightness, const float base_brightness, const float prev_base_brightness, const float decay);

constexpr inline float clerp(float edge0, float edge1, float x);
constexpr inline float clerp2(float edge0, float edge1, float edg2, float edge3, float x);

float get_division_sum(float center, float in_len, float out_len, vector<float>* magnitudes);

float get_bin_sum(const float start, const float end, vector<float>* magnitudes);

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

struct Algo_FBDGM : public SpectrumAlgo{
    public:

        virtual void base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes);

        virtual void transform_algo(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes){};

        virtual void init(){};
};