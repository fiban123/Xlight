#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>

using namespace std;

struct GUI{
    sf::VideoMode mode;
    sf::RenderWindow window;
    size_t fps;

    bool running;

    thread gui_thread;

    void init(size_t fps, sf::VideoMode mode);

    void start();
    void stop();
    void wait_for_exit();

    void loop();

    ~GUI();
};
