#include "../include/math.hpp"

constexpr inline float nmap(float value, float lo1, float hi1, float lo2, float hi2) {
    return lo2 + (value - lo1) * (hi2 - lo2) / (hi1 - lo1);
}

inline float vecmax(vector<float>& v){
    float max = v[0];
    for(float i : v){
        if(i > max){
            max = i;
        }
    }
    return max;
}

inline float vecavg(vector<float>& v){
    float sum = 0;
    for(float i : v){
        sum += i;
    }
    return sum / v.size();
}

inline float dequeavg(deque<float>& d) {

    float sum = 0;
    for (const float& i : d){
        sum += i;
    }

    return sum / d.size();
}
