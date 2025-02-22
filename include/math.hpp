#pragma once

#include <vector>
#include <deque>

using namespace std;

constexpr inline float nmap(float value, float lo1, float hi1, float lo2, float hi2);

inline float vecmax(vector<float>& v);

inline float vecavg(vector<float>& v);

inline float dequeavg(deque<float>& d);
