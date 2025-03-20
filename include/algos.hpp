#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <algorithm>

#include "../include/math.hpp"

using namespace std;




struct VisualizationAlgorithm{ // abstract struct
    virtual void execute() = 0;
    
    virtual void init() = 0;

    // helpers
    
    inline float get_division_sum(float center, float in_len, float out_len);
    
    inline float get_freq_bin(float freq);


    float& sample_ratio;
    array<sf::Color, 4>& channels;
    vector<float>& spectrogram;

    VisualizationAlgorithm(float& sample_ratio, array<sf::Color, 4>& channels, vector<float>& spectrogram);

    VisualizationAlgorithm() = delete;
};

struct FBDGM : VisualizationAlgorithm{
    using VisualizationAlgorithm::VisualizationAlgorithm;

    void execute();

    void init();

    float bass_start = 0.0f;
    float bass_d1 = 40.0f;
    float bass_d2 = 100.0f;
    float bass_d3 = 200.0f;

    float mids_d1 = bass_d3;
    float mids_d2 = 1300.0f;
    float mids_d3 = 4500.0f;

    float highs_d1 = 5500.0f;
    float highs_d2 = 12000.0f;
    float highs_d3 = 22000.0f;


    float bass_d1_base_quotient = 7.0f;
    float bass_d2_base_quotient = 9.0f;
    float bass_d3_base_quotient = bass_d1_base_quotient;

    float mids_d1_base_quotient = 30.0f;
    float mids_d2_base_quotient = mids_d1_base_quotient;
    float mids_d3_base_quotient = mids_d1_base_quotient;

    float highs_d1_base_quotient = 20.0f;
    float highs_d2_base_quotient = highs_d1_base_quotient;
    float highs_d3_base_quotient = highs_d1_base_quotient;

    float harmonics_d1_quotient = 4.0f;
    float harmonics_d2_quotient = 4.0f;
    float harmonics_d3_quotient = 1.5f;


    float harmonics_d1 = 300.0f;
    float harmonics_d2 = 2000.0f;
    float harmonics_d3 = 12000.0f;
    float harmonics_end = 22000.0f;
    float harmonics_quotient = 6.0f;


    float bass_d1_quotient;
    float bass_d2_quotient;
    float bass_d3_quotient;

    float mids_d1_quotient;
    float mids_d2_quotient;
    float mids_d3_quotient;

    float highs_d1_quotient;
    float highs_d2_quotient;
    float highs_d3_quotient;

    float bass_d1_in_len;
    float bass_d1_out_len;
    float bass_d2_in_len;
    float bass_d2_out_len;
    float bass_d3_in_len;
    float bass_d3_out_len;

    float mids_d1_in_len;
    float mids_d1_out_len;
    float mids_d2_in_len;
    float mids_d2_out_len;
    float mids_d3_in_len;
    float mids_d3_out_len;

    float highs_d1_in_len;
    float highs_d1_out_len;
    float highs_d2_in_len;
    float highs_d2_out_len;
    float highs_d3_in_len;
    float highs_d3_out_len;

    float harmonics_d1_in_len;
    float harmonics_d1_out_len;
    float harmonics_d2_in_len;
    float harmonics_d2_out_len;
    float harmonics_d3_in_len;
    float harmonics_d3_out_len;
};

enum Algo{
    ALGO_FBDGM,
};