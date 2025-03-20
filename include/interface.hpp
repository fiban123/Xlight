#pragma once

#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
#include <string>
#include "algos.hpp"

using namespace std;

class TerminalInterface{
    public:
        VisualizationAlgorithm* algo;
        thread interface_thread;
        bool running = false;

        void init(VisualizationAlgorithm* algptr);

        void start();
        void stop();

        vector<string> parse_cmd(string cmd);

        void interface_loop();

        ~TerminalInterface();
};