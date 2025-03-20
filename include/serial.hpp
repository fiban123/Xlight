#pragma once

#include <libftdi1/ftdi.h>
#include <SFML/Graphics.hpp>
#include "math.hpp"
#include <vector>
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;



class FT232R{
    public:
    FT232R();
    
    inline void busy_wait(unsigned long int ns);
    
    struct ftdi_context *ctx;
    
    inline int set_break(ftdi_break_type state);
    
    void write_dmx_frame(vector<unsigned char> data);
    
    ~FT232R();
};

class DMXInterface{
    public:
        FT232R serial_ifc;

        bool running = false;

        const array<sf::Color, 4>* channels;

        thread serial_thread;

        void start();
        void stop();

        void loop();

        ~DMXInterface();
};