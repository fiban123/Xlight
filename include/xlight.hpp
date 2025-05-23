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
#include "serial.hpp"
#include "interface.hpp"

#include <vector>
#include <functional>


using namespace std;


class Xlight{
    public:

        Xlight(bool init = true) : dmx(init){}
        // GUI
        GUI gui; // must be initialized before starting

        DMXInterface dmx;  // must be initialized before starting

        TerminalInterface interface;
};