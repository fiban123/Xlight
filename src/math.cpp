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