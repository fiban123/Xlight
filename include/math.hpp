#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include <math.h>

using namespace std;

constexpr inline float nmap(float value, float lo1, float hi1, float lo2, float hi2);

inline float vecmax(vector<float>& v);

inline float vecavg(vector<float>& v);

inline float dequeavg(deque<float>& d);

inline float normalize_color(float value);

constexpr inline float clerp(float edge0, float edge1, float x);
inline constexpr float clerp2(float edge0, float edge1, float edge2, float edge3, float x);
