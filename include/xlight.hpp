#pragma once
/*
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <memory>
#include <algorithm>
#include <thread>


#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>


#include <SFML/Graphics.hpp>
#include <fftw3.h>
#include <portaudio.h>
*/

#include "config.hpp"
#include "stream.hpp"
#include "gui.hpp"

#include <vector>
#include <functional>


using namespace std;


class Xlight{
    public:
        // GUI
        GUI gui; // must be initialized before starting

};