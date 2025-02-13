#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <vector>
#include <array>
#include <numeric>
#include <iomanip>
#include <map>
#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>
#include <queue>

#include <fftw3.h>
#include <portaudio.h>

// include raylib in namespace to avoid windows conflicts
namespace rl{
    #include "raylib.h"
}

// include windows in a namespace to avoid conflict with raylib
#define WIN32_LEAN_AND_MEAN
#define NODRAWTEXT
namespace windows{
    #include <windows.h>
    #include <mmdeviceapi.h>
    #include <endpointvolume.h>
}

using namespace std;;

#define CHECKERR(err) if (err != paNoError){ cout << "PA error: " << Pa_GetErrorText(err) << endl; exit(1);}
#define CHECKSIGN(x) if (x < 0){ cout << "Sign Error" << endl; exit(1); }
#define CHECKZERO(x) if (x == 0) { cout << "Zero Error" << endl; exit(1); }

#include "config.hpp"

#include "channels.hpp"
#include "algos.hpp"
#include "stream.hpp"
#include "fft.hpp"
#include "gui.hpp"

#include "channels.cpp"
#include "algos.cpp"
#include "stream.cpp"
#include "fft.cpp"
#include "gui.cpp"