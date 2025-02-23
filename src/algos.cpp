#include "../include/algos.hpp"

inline float VisualizationAlgorithm::get_division_sum(float center, float in_len, float out_len) {
    float sum = 0.0f;

    for (unsigned int i = get_freq_bin(center - in_len); i < get_freq_bin(center + out_len); i++){
        float freq = i * sample_ratio;

        float weight = clerp2(center - in_len, center, center, center + out_len, freq);

        sum += weight * spectrogram[i];
    }

    return sum;
}

inline float VisualizationAlgorithm::get_freq_bin(float freq) {
    return freq / sample_ratio;
}

VisualizationAlgorithm::VisualizationAlgorithm(float& sample_ratio, array<sf::Color, 4>& channels, vector<float>& spectrogram)
: sample_ratio(sample_ratio), channels(channels), spectrogram(spectrogram) {}


void FBDGM::execute() {
        // mids (channel 1)
        float mids_d1_sum = get_division_sum(mids_d1, mids_d1_in_len, mids_d1_out_len);
        float mids_d2_sum = get_division_sum(mids_d2, mids_d2_in_len, mids_d2_out_len);
        float mids_d3_sum = get_division_sum(mids_d3, mids_d3_in_len, mids_d3_out_len);
    
        channels[1].r = (uint8_t)clamp(mids_d1_sum / mids_d1_quotient, 0.0f, 255.0f);
        channels[1].g = (uint8_t)clamp(mids_d2_sum / mids_d2_quotient, 0.0f, 255.0f);
        channels[1].b = (uint8_t)clamp(mids_d3_sum / mids_d3_quotient, 0.0f, 255.0f);
    
        // highs (channel 2)
        float highs_d1_sum = get_division_sum(highs_d1, highs_d1_in_len, highs_d1_out_len);
        float highs_d2_sum = get_division_sum(highs_d2, highs_d2_in_len, highs_d2_out_len);
        float highs_d3_sum = get_division_sum(highs_d3, highs_d3_in_len, highs_d3_out_len);

        channels[2].r = (uint8_t)clamp(highs_d1_sum / highs_d1_quotient, 0.0f, 255.0f);
        channels[2].g = (uint8_t)clamp(highs_d2_sum / highs_d2_quotient, 0.0f, 255.0f);
        channels[2].b = (uint8_t)clamp(highs_d3_sum / highs_d3_quotient, 0.0f, 255.0f);
    
        // bass (C0)
        float bass_d1_sum = get_division_sum(bass_d1, bass_d1_in_len, bass_d1_out_len);
        float bass_d2_sum = get_division_sum(bass_d2, bass_d2_in_len, bass_d2_out_len);
        float bass_d3_sum = get_division_sum(bass_d3, bass_d3_in_len, bass_d3_out_len);

        channels[0].r = (uint8_t)clamp(bass_d1_sum / bass_d1_quotient, 0.0f, 255.0f);
        channels[0].g = (uint8_t)clamp(bass_d2_sum / bass_d2_quotient, 0.0f, 255.0f);
        channels[0].b = (uint8_t)clamp(bass_d3_sum / bass_d3_quotient, 0.0f, 255.0f);
    
        // peakiness / harmonics (C3)
    
        // get average frequency
        float avg_magnitude = 0.0f;
        for (unsigned int i = get_freq_bin(harmonics_start); i < spectrogram.size(); i++){
            
            avg_magnitude += spectrogram[i];
        }
        avg_magnitude /= (spectrogram.size() - get_freq_bin(harmonics_start));
    
        float peakiness = 0.0f;
    
        for (unsigned int i = get_freq_bin(harmonics_start); i < spectrogram.size() - 3; i++){
            
            //peakiness += pow(max(0.0f, magnitudes->at(i) - (avg_magnitude * 4.0f + 150.0f)), 1.1f);
            //peakiness += pow(max(0.0f, magnitudes->at(i) - avg_magnitude) * magnitudes->at(i) * 0.001f, 1.1f);
    
            float local_avg_magnitude = 0.0f;
    
            local_avg_magnitude += spectrogram[i - 2];
            local_avg_magnitude += spectrogram[i - 3];
    
            local_avg_magnitude += spectrogram[i + 2];
            local_avg_magnitude += spectrogram[i + 3];
    
            local_avg_magnitude /= 4.0f;
    
            float diff = local_avg_magnitude - spectrogram[i];
    
            peakiness += max(0.0f, diff - 15.0f);
        }
    
        // 1'
        channels[3].r = (uint8_t)clamp(peakiness / 6.0f, 0.0f, 255.0f);
}

void FBDGM::init() {
    bass_d1_quotient = bass_d1_base_quotient;
    bass_d2_quotient = bass_d2_base_quotient;
    bass_d3_quotient = bass_d3_base_quotient;

    mids_d1_quotient = mids_d1_base_quotient / 2.0f;
    mids_d2_quotient = mids_d2_base_quotient;
    mids_d3_quotient = mids_d3_base_quotient;

    highs_d1_quotient = highs_d1_base_quotient;
    highs_d2_quotient = highs_d2_base_quotient;
    highs_d3_quotient = highs_d3_base_quotient / 2.0f;
    

    bass_d1_in_len = 0.0f;
    bass_d1_out_len = (bass_d2 - bass_d1);
    bass_d2_in_len = bass_d1_out_len;
    bass_d2_out_len = (bass_d3 - bass_d2);
    bass_d3_in_len = bass_d2_out_len;
    bass_d3_out_len = 0.0f;

    mids_d1_in_len = 0.0f;
    mids_d1_out_len = (mids_d2 - mids_d1);
    mids_d2_in_len = mids_d1_out_len;
    mids_d2_out_len = (mids_d3 - mids_d2);
    mids_d3_in_len = mids_d2_out_len;
    mids_d3_out_len = (highs_d1 - mids_d3);

    highs_d1_in_len = mids_d3_out_len;
    highs_d1_out_len = (highs_d2 - highs_d1);
    highs_d2_in_len = highs_d1_out_len;
    highs_d2_out_len = (highs_d3 - highs_d2);
    highs_d3_in_len = highs_d2_out_len;
    highs_d3_out_len = 0.0f;
}