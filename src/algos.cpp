// helper functions

constexpr inline float smoothstep(float edge0, float edge1, float x) {
    // Clamp x to the range [0, 1]
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Perform the smoothstep interpolation
    return x * x * (3.0f - 2.0f * x);
}

constexpr inline float smoothstep2(float edge0_0, float edge1_0, float edge0_1, float edge1_1, float x){
    if (x < edge0_0 || x > edge1_1){
        return 0.0f;
    } else if (x > edge1_0 && x < edge0_1){
        return 1.0f;
    } else if (x > edge0_0 && x < edge1_0){
        return smoothstep(edge0_0, edge1_0, x);
    }
    return smoothstep(edge0_1, edge1_1, x);
}

const inline unsigned int get_magnitude_bin(float freq){
    return freq / sample_ratio + (float) start_bin_index;
}

constexpr inline float get_spiked_brightness(float spiked_brightness, const float base_brightness, const float prev_base_brightness, const float decay){
    float brightness_derivative = max(0.0f, base_brightness - prev_base_brightness);
    spiked_brightness += nmap(sqrtf(brightness_derivative), 0.0f, 15.96f, 0.0f, 255.0f);
    spiked_brightness -= decay;
    return clamp(spiked_brightness, 0.0f, 255.0f);
}

constexpr inline float clerp(float edge0, float edge1, float x) {
    return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

inline constexpr float clerp2(float edge0, float edge1, float edge2, float edge3, float x) {
    if (x < edge0 || x > edge3){
        return 0.0f;
    } else if (x > edge1 && x < edge2){
        return 1.0f;
    } else if (x > edge0 && x < edge1){
        return clerp(edge0, edge1, x);
    }
    return clerp(edge3, edge2, x);
}

inline float get_division_sum(float center, float in_len, float out_len, vector<float>* magnitudes){
    float sum = 0.0f;

    for (unsigned int i = get_magnitude_bin(center - in_len); i < get_magnitude_bin(center + out_len); i++){
        float freq = i * sample_ratio;

        float weight = clerp2(center - in_len, center, center, center + out_len, freq);

        sum += weight * magnitudes->at(i);
    }

    return sum;
}

inline float get_bin_sum(const float start, const float end, vector<float>* magnitudes){
    float sum = 0.0f;
    
    for (unsigned int i = get_magnitude_bin(start); i < get_magnitude_bin(end); i++){
        float freq = i * sample_ratio;

        sum += magnitudes->at(i);
    }

    return sum;
}

// algorithms

void Algo_FBGM::base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes) {
    // C0 -> bass, static hue
    // C1 -> mids, dominant frequency based hue determination
    // C2 -> highs, dominant frequency based hue determination
    // C3 -> spiked total volume

    // C0, sum up all bass magnitudes
    float bass_sum = 0.0f;
    for (unsigned int i = get_magnitude_bin(FBGM_BASS_START); i < get_magnitude_bin(FBGM_BASS_G2_START); i++){
        float freq = i * sample_ratio;

        bass_sum += smoothstep(FBGM_BASS_G2_END, FBGM_BASS_G2_START, freq) * magnitudes->at(i);
    }
    base_channels->at(0).col.r = (uint8_t)clamp(bass_sum / FBGM_BASS_FACTOR, 0.0f, 255.0f);



    // C1, sum up all mids magnitudes, calculate dominant frequency
    float mids_sum = 0.0f;
    float mids_weighted_frequency_sum = 0.0f;
    float mids_weight_sum = 0.0f;

    for (unsigned int i = get_magnitude_bin(FBGM_MIDS_G1_START); i < get_magnitude_bin(FBGM_MIDS_G2_END); i++){
        float freq = i * sample_ratio;

        mids_sum += smoothstep2(FBGM_MIDS_G1_START, FBGM_MIDS_G1_END, FBGM_MIDS_G2_START, FBGM_MIDS_G2_END, freq) * magnitudes->at(i);

        mids_weighted_frequency_sum += /*smoothstep(FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_G1_END, freq) * */ freq * (pow(magnitudes->at(i), 2) / FBGM_MIDS_FLAOT_OVERFLOW_FACTOR); // square this to suppress noise
        mids_weight_sum += (pow(magnitudes->at(i), 2) / FBGM_MIDS_FLAOT_OVERFLOW_FACTOR);
    }

    // normalize dominant frequency

    float mids_dominant_frequency = mids_weighted_frequency_sum / mids_weight_sum;

    float normalized_mids_sum = mids_sum / FBGM_MIDS_FACTOR;
    float mids_normalized_dominant_frequency = mids_dominant_frequency - FBGM_MIDS_HD_G1_START;
    mids_normalized_dominant_frequency = sqrtf(mids_normalized_dominant_frequency);
    mids_normalized_dominant_frequency = nmap(mids_normalized_dominant_frequency, 0.0f, sqrt(FBGM_MIDS_HD_END - FBGM_MIDS_HD_G1_START), 0.0f, 360.0f);

    // base brightness is defined by amplitude / volume, hue is defined by the dominant frequency.
    float mids_brightness = clamp(normalized_mids_sum, 0.0f, 255.0f);

    base_channels->at(1).col.r = (uint8_t) mids_brightness;
    base_channels->at(1).col.g = (uint8_t)0;
    base_channels->at(1).col.b = (uint8_t)0;
    // hue is shifted by the normalized dominant frequency.
    base_channels->at(1).col.shift_hue(mids_normalized_dominant_frequency);



    // C2, sum up all magnitudes
    float highs_sum = 0.0f;
    float highs_weighted_frequency_sum = 0.0f;
    float highs_weight_sum = 0.0f;
    
    for (unsigned int i = get_magnitude_bin(FBGM_HIGHS_G1_START); i < get_magnitude_bin(FBGM_HIGHS_END); i++){
        float freq = i * sample_ratio;
        
        highs_sum += smoothstep(FBGM_HIGHS_G1_START, FBGM_HIGHS_G1_END, freq) * magnitudes->at(i);

        highs_weighted_frequency_sum += freq * (pow(magnitudes->at(i), 2) / FBGM_HIGHS_FLOAT_OVERFLOW_FACTOR);
        highs_weight_sum += (pow(magnitudes->at(i), 2) / FBGM_HIGHS_FLOAT_OVERFLOW_FACTOR);
    }

    float highs_dominant_frequency = highs_weighted_frequency_sum / highs_weight_sum;

    float normalized_highs_sum = highs_sum / FBGM_HIGHS_FACTOR;
    float highs_normalized_dominant_frequency = highs_dominant_frequency - FBGM_HIGHS_HD_START;
    highs_normalized_dominant_frequency = sqrt(highs_normalized_dominant_frequency);
    highs_normalized_dominant_frequency = nmap(highs_normalized_dominant_frequency, 0.0f, sqrt(FBGM_HIGHS_HD_END - FBGM_HIGHS_HD_START), 0.0f, 360.0f);

    lines = {mids_dominant_frequency, highs_dominant_frequency, FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_END, FBGM_HIGHS_HD_START, FBGM_HIGHS_HD_END};

    base_channels->at(2).col.r = (uint8_t)clamp(normalized_highs_sum, 0.0f, 255.0f);
    base_channels->at(2).col.g = (uint8_t)0;
    base_channels->at(2).col.b = (uint8_t)0;
    base_channels->at(2).col.shift_hue(highs_normalized_dominant_frequency);

    //base_channels->at(2).prev_amp = normalized_highs_sum;
}




void Algo_SFBGM::base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes) {
    // C0 -> spiked bass, static hue
    // C1 -> spiked mids, dominant frequency based hue determination
    // C2 -> spiked highs, dominant frequency based hue determination
    // C3 -> spiked total volume (=FBGM)

    // C0, sum up all bass magnitudes
    float bass_sum = 0.0f;
    for (unsigned int i = get_magnitude_bin(FBGM_BASS_START); i < get_magnitude_bin(FBGM_BASS_G2_START); i++){
        float freq = i * sample_ratio;

        bass_sum += smoothstep(FBGM_BASS_G2_END, FBGM_BASS_G2_START, freq) * magnitudes->at(i);
    }
    float normalized_bass_sum = bass_sum / FBGM_BASS_FACTOR;
    float bass_base_brightness = clamp(normalized_bass_sum, 0.0f, 255.0f);

    bass_spiked_brightness = get_spiked_brightness(bass_spiked_brightness, bass_base_brightness, prev_bass_base_brightness, SFBGM_BASS_SPIKE_DECAY_FACTOR);
    prev_bass_base_brightness = bass_base_brightness;

    uint8_t bass_combined_brightness = (uint8_t)clamp(bass_base_brightness * 0.7f + bass_spiked_brightness * 0.3f, 0.0f, 255.0f);
    
    base_channels->at(0).col.r = bass_combined_brightness;

    // C1, sum up all mids magnitudes, calculate dominant frequency
    float mids_sum = 0.0f;
    float mids_weighted_frequency_sum = 0.0f;
    float mids_weight_sum = 0.0f;

    for (unsigned int i = get_magnitude_bin(FBGM_MIDS_G1_START); i < get_magnitude_bin(FBGM_MIDS_G2_END); i++){
        float freq = i * sample_ratio;

        mids_sum += smoothstep2(FBGM_MIDS_G1_START, FBGM_MIDS_G1_END, FBGM_MIDS_G2_START, FBGM_MIDS_G2_END, freq) * magnitudes->at(i);

        // magnitudfe is squread to suppress noise
        mids_weighted_frequency_sum += /*smoothstep(FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_G1_END, freq) * */ freq * (pow(magnitudes->at(i), 2) / FBGM_MIDS_FLAOT_OVERFLOW_FACTOR);
        mids_weight_sum += (pow(magnitudes->at(i), 2) / FBGM_MIDS_FLAOT_OVERFLOW_FACTOR);

    }

    // normalize dominant frequency

    float mids_dominant_frequency = mids_weighted_frequency_sum / mids_weight_sum;

    float normalized_mids_sum = mids_sum / FBGM_MIDS_FACTOR;
    float mids_normalized_dominant_frequency = mids_dominant_frequency - FBGM_MIDS_HD_G1_START;
    mids_normalized_dominant_frequency = sqrtf(mids_normalized_dominant_frequency);
    mids_normalized_dominant_frequency = nmap(mids_normalized_dominant_frequency, 0.0f, sqrt(FBGM_MIDS_HD_END - FBGM_MIDS_HD_G1_START), 0.0f, 360.0f);

    // base brightness is defined by amplitude / volume, hue is defined by the dominant frequency.
    float mids_base_brightness = clamp(normalized_mids_sum, 0.0f, 255.0f);

    mids_spiked_brightness = get_spiked_brightness(mids_spiked_brightness, mids_base_brightness, prev_mids_base_brightness, SFBGM_MIDS_SPIKE_DECAY_FACTOR);
    prev_mids_base_brightness = mids_base_brightness;

    uint8_t mids_combined_brightness = (uint8_t)clamp(mids_base_brightness * 0.7f + mids_spiked_brightness * 0.3f, 0.0f, 255.0f);

    base_channels->at(1).col.r = (uint8_t) mids_combined_brightness;
    base_channels->at(1).col.g = (uint8_t) 0;
    base_channels->at(1).col.b = (uint8_t) 0;
    // hue is shifted by the normalized dominant frequency.
    base_channels->at(1).col.shift_hue(mids_normalized_dominant_frequency);



    // C2, sum up all magnitudes
    float highs_sum = 0.0f;
    float highs_weighted_frequency_sum = 0.0f;
    float highs_weight_sum = 0.0f;
    
    for (unsigned int i = get_magnitude_bin(FBGM_HIGHS_G1_START); i < get_magnitude_bin(FBGM_HIGHS_END); i++){
        float freq = i * sample_ratio;
        
        highs_sum += smoothstep(FBGM_HIGHS_G1_START, FBGM_HIGHS_G1_END, freq) * magnitudes->at(i);

        highs_weighted_frequency_sum += freq * (pow(magnitudes->at(i), 2) / FBGM_HIGHS_FLOAT_OVERFLOW_FACTOR);
        highs_weight_sum += (pow(magnitudes->at(i), 2) / FBGM_HIGHS_FLOAT_OVERFLOW_FACTOR);
    }

    float highs_dominant_frequency = highs_weighted_frequency_sum / highs_weight_sum;

    float normalized_highs_sum = highs_sum / FBGM_HIGHS_FACTOR;
    float highs_normalized_dominant_frequency = highs_dominant_frequency - FBGM_HIGHS_HD_START;
    highs_normalized_dominant_frequency = sqrt(highs_normalized_dominant_frequency);
    highs_normalized_dominant_frequency = nmap(highs_normalized_dominant_frequency, 0.0f, sqrt(FBGM_HIGHS_HD_END - FBGM_HIGHS_HD_START), 0.0f, 360.0f);

    float highs_base_brightness = clamp(normalized_highs_sum, 0.0f, 255.0f);

    highs_spiked_brightness = get_spiked_brightness(highs_spiked_brightness, highs_base_brightness, prev_highs_base_brightness, SFBGM_HIGHS_SPIKE_DECAY_FACTOR);
    prev_highs_base_brightness = highs_base_brightness;

    uint8_t highs_combined_brightness = (uint8_t)clamp(highs_base_brightness * 0.7f + highs_spiked_brightness * 0.3f, 0.0f, 255.0f);

    base_channels->at(2).col.r = highs_combined_brightness;
    base_channels->at(2).col.g = (uint8_t)0;
    base_channels->at(2).col.b = (uint8_t)0;
    base_channels->at(2).col.shift_hue(highs_normalized_dominant_frequency);


    lines = {mids_dominant_frequency, highs_dominant_frequency, FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_END, FBGM_HIGHS_HD_START, FBGM_HIGHS_HD_END};
}

void Algo_FBDGM::base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes) {

    // mids (channel 1)
    float mids_d1_sum = get_division_sum(FBDGM_MIDS_D1, 0.0f, FBDGM_MIDS_D1_OUT_LEN, magnitudes);
    float mids_d2_sum = get_division_sum(FBDGM_MIDS_D2, FBDGM_MIDS_D2_IN_LEN, FBDGM_MIDS_D2_OUT_LEN, magnitudes);
    float mids_d3_sum = get_division_sum(FBDGM_MIDS_D3, FBDGM_MIDS_D3_IN_LEN, FBDGM_MIDS_D3_OUT_LEN, magnitudes);

    base_channels->at(1).col.r = (uint8_t)clamp(mids_d1_sum / FBDGM_MIDS_D1_FACTOR, 0.0f, 255.0f);
    base_channels->at(1).col.g = (uint8_t)clamp(mids_d2_sum / FBDGM_MIDS_D2_FACTOR, 0.0f, 255.0f);
    base_channels->at(1).col.b = (uint8_t)clamp(mids_d3_sum / FBDGM_MIDS_D3_FACTOR, 0.0f, 255.0f);

    // highs (channel 2)
    float highs_d1_sum = get_division_sum(FBDGM_HIGHS_D1, FBDGM_HIGHS_D1_IN_LEN, FBDGM_HIGHS_D1_OUT_LEN, magnitudes);
    float highs_d2_sum = get_division_sum(FBDGM_HIGHS_D2, FBDGM_MIDS_D2_IN_LEN, FBDGM_HIGHS_D2_OUT_LEN, magnitudes);
    float highs_d3_sum = get_division_sum(FBDGM_HIGHS_D3, FBDGM_MIDS_D3_IN_LEN, 0.0f, magnitudes);

    base_channels->at(2).col.r = (uint8_t)clamp(highs_d1_sum / FBDGM_HIGHS_D1_FACTOR, 0.0f, 255.0f);
    base_channels->at(2).col.g = (uint8_t)clamp(highs_d2_sum / FBDGM_HIGHS_D2_FACTOR, 0.0f, 255.0f);
    base_channels->at(2).col.b = (uint8_t)clamp(highs_d3_sum / FBDGM_HIGHS_D3_FACTOR, 0.0f, 255.0f);

    // bass
    float bass_d1_sum = get_division_sum(FBDGM_BASS_D1, FBDGM_BASS_D1_IN_LEN, FBDGM_BASS_D1_OUT_LEN, magnitudes);
    float bass_d2_sum = get_division_sum(FBDGM_BASS_D2, FBDGM_BASS_D2_IN_LEN, FBDGM_BASS_D2_OUT_LEN, magnitudes);
    float bass_d3_sum = get_division_sum(FBDGM_BASS_D3, FBDGM_BASS_D3_IN_LEN, FBDGM_BASS_D3_OUT_LEN, magnitudes);

    base_channels->at(0).col.r = (uint8_t)clamp(bass_d1_sum / FBDGM_BASS_D1_FACTOR, 0.0f, 255.0f);
    base_channels->at(0).col.g = (uint8_t)clamp(bass_d2_sum / FBDGM_BASS_D2_FACTOR, 0.0f, 255.0f);
    base_channels->at(0).col.b = (uint8_t)clamp(bass_d3_sum / FBDGM_BASS_D3_FACTOR, 0.0f, 255.0f);


    lines = {FBDGM_MIDS_D1, FBDGM_MIDS_D2, FBDGM_MIDS_D3, FBDGM_HIGHS_D1, FBDGM_HIGHS_D2, FBDGM_HIGHS_D3};
}
