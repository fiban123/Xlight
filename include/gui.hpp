#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>
#include <math.h>

#include "../include/math.hpp"
#include "../include/stream.hpp"

using namespace std;

enum gui_mode{
    MODE_SPECTROGRAM,
    MODE_SPECTROGRAM_DERIVATIVE,

    MODE_COUNT // do not remove this !!
};

struct Graph{
    sf::Vector2f pos;
    float width, height;

    float min_x, max_x;
    float min_y, max_y;

    sf::VertexArray vertices;
    sf::VertexArray bounding_vertices;

    void init(sf::Vector2f pos, float width, float height, float min_x, float max_x, float min_y, float max_y);

    void add_linear_spline(const vector<sf::Vector2f>& points);

    void add_bars(const vector<sf::Vector2f>& points, float neutral);

    void draw(sf::RenderWindow& window);
};

struct GUI{
    AudioStream stream;

    gui_mode mode = MODE_SPECTROGRAM;

    sf::VideoMode window_mode;
    sf::RenderWindow window;
    size_t fps;

    bool running;

    thread gui_thread;

    float max_magnitude;
    float spectrogram_length;

    Graph spectrogram_graph;
    Graph spectrogram_derivative_graph;

    //void init(size_t _fps, sf::VideoMode _mode, float _max_magnitude);

    void init(size_t _fps, sf::VideoMode _mode, float _max_magnitude, size_t frames_per_buffer, size_t frames_per_fft, PaDeviceIndex device, 
        size_t spectrogram_factor_update_rate, function<void(float*)> spectrogram_factor_update_func);

    void start();
    void stop();
    void wait_for_exit();

    void loop();

    ~GUI();
};
