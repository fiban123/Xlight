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

constexpr inline unsigned int get_magnitude_bin(float freq){
    return freq / sample_ratio + start_bin_index;
}

void Algo_FBGM::base_algo(array<Channel, N_CHANNELS>* base_channels, vector<float>* magnitudes) {
    // C0 -> bass, static hue
    // C1 -> mids, dominant frequency based hue determination
    // C2 -> highs, dominant frequency based hue determination
    // C3 -> positive rate of change in volume

    // C0, sum up all bass magnitudes
    float bass_sum = 0.0f;
    for (unsigned int i = get_magnitude_bin(FBGM_BASS_START); i < get_magnitude_bin(FBGM_BASS_G2_START); i++){
        float freq = i * sample_ratio;

        bass_sum += smoothstep(FBGM_BASS_G2_END, FBGM_BASS_G2_START, freq) * magnitudes->at(i);
    }
    base_channels->at(0).col.r = (uint8_t)clamp(bass_sum / 10, 0.0f, 255.0f);

    // C1, sum up all magnitudes, calculate dominant frequency
    float mids_sum = 0.0f;
    float mids_weighted_frequency_sum = 0.0f;
    float mids_weight_sum = 0.0f;

    for (unsigned int i = get_magnitude_bin(FBGM_MIDS_G1_START); i < get_magnitude_bin(FBGM_MIDS_G2_END); i++){
        float freq = i * sample_ratio;

        mids_sum += smoothstep2(FBGM_MIDS_G1_START, FBGM_MIDS_G1_END, FBGM_MIDS_G2_START, FBGM_MIDS_G2_END, freq) * magnitudes->at(i);

        mids_weighted_frequency_sum += /*smoothstep(FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_G1_END, freq) * */ freq * (pow(magnitudes->at(i), 2) / 100);
        mids_weight_sum += (pow(magnitudes->at(i), 2) / 100);
    }


    float mids_dominant_frequency = mids_weighted_frequency_sum / mids_weight_sum;


    float normalized_mids_sum = mids_sum / 30.0f;
    float mids_normalized_dominant_frequency = mids_dominant_frequency - FBGM_MIDS_HD_G1_START;
    mids_normalized_dominant_frequency = sqrt(mids_normalized_dominant_frequency);
    mids_normalized_dominant_frequency = nmap(mids_normalized_dominant_frequency, 0.0f, sqrt(FBGM_MIDS_HD_END - FBGM_MIDS_HD_G1_START), 0.0f, 360.0f);

    base_channels->at(1).col.r = (uint8_t)clamp(normalized_mids_sum, 0.0f, 255.0f);
    base_channels->at(1).col.g = (uint8_t)0;
    base_channels->at(1).col.b = (uint8_t)0;
    base_channels->at(1).col.shift_hue(mids_normalized_dominant_frequency);

    // C2, sum up all magnitudes
    float highs_sum = 0.0f;
    float highs_weighted_frequency_sum = 0.0f;
    float highs_weight_sum = 0.0f;
    
    for (unsigned int i = get_magnitude_bin(FBGM_HIGHS_G1_START); i < get_magnitude_bin(FBGM_HIGHS_END); i++){
        float freq = i * sample_ratio;
        
        highs_sum += smoothstep(FBGM_HIGHS_G1_START, FBGM_HIGHS_G1_END, freq) * magnitudes->at(i);

        highs_weighted_frequency_sum += freq * (pow(magnitudes->at(i), 2) / 30);
        highs_weight_sum += (pow(magnitudes->at(i), 2) / 30);
    }

    float highs_dominant_frequency = highs_weighted_frequency_sum / highs_weight_sum;

    lines = {mids_dominant_frequency, highs_dominant_frequency, FBGM_MIDS_HD_G1_START, FBGM_MIDS_HD_END, FBGM_HIGHS_HD_START, FBGM_HIGHS_HD_END};

    float normalized_highs_sum = highs_sum / 60.0f;
    float highs_normalized_dominant_frequency = highs_dominant_frequency - FBGM_HIGHS_HD_START;
    highs_normalized_dominant_frequency = sqrt(highs_normalized_dominant_frequency);
    highs_normalized_dominant_frequency = nmap(highs_normalized_dominant_frequency, 0.0f, sqrt(FBGM_HIGHS_HD_END - FBGM_HIGHS_HD_START), 0.0f, 360.0f);

    //float c2_amp_pdelta = clamp((normalized_highs_sum / base_channels->at(2).prev_amp - 1.0f) * 255.0f, 1.0f, 255.0f); // rate of positive change in c1_amp

    //cout << c2_amp_pdelta << endl;

    //float c2_amp = normalized_highs_sum * 0.7f + c2_amp_pdelta * 0.3f;

    base_channels->at(2).col.r = (uint8_t)clamp(normalized_highs_sum, 0.0f, 255.0f);
    base_channels->at(2).col.g = (uint8_t)0;
    base_channels->at(2).col.b = (uint8_t)0;
    base_channels->at(2).col.shift_hue(highs_normalized_dominant_frequency);

    //base_channels->at(2).prev_amp = normalized_highs_sum;
}

void Algo_FBGM::transform_algo(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes) {

}