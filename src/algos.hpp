constexpr inline float smoothstep(float edge0, float edge1, float x);

constexpr inline float smoothstep2(float edge0_0, float edge1_0, float edge0_1, float edge1_1, float x);

const inline unsigned int get_magnitude_bin(float freq);

constexpr inline float get_spiked_brightness(float spiked_brightness, const float base_brightness, const float prev_base_brightness, const float decay);