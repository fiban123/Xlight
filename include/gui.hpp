#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>
#include <math.h>
#include <memory>

#include "../include/math.hpp"
#include "../include/stream.hpp"
#include "../include/algos.hpp"

using namespace std;

struct guiStates{
    bool draw_spectrogram = true;
    bool draw_light_preview = true;
    bool draw_info = true;
};

struct Marker{
    float c;
    float len;
    int text_size;
    string text;

    sf::Text rtext;
};

struct Graph{
    sf::Vector2f pos;
    float width, height;

    float min_x, max_x;
    float min_y, max_y;

    sf::VertexArray vertices;
    sf::VertexArray bounding_vertices;

    sf::VertexArray marker_vertices;
    vector<Marker> x_markers;

    void init(sf::Vector2f pos, float width, float height, float min_x, float max_x, float min_y, float max_y);

    void add_linear_spline(const vector<sf::Vector2f>& points);

    void add_bars(const vector<sf::Vector2f>& points, float neutral);

    void add_markers();

    void draw(sf::RenderWindow& window);
};

struct GUI{
    guiStates state;

    array<sf::Color, 4> channels;
    unique_ptr<VisualizationAlgorithm> algo;

    AudioStream stream;

    sf::VideoMode window_mode;
    sf::RenderWindow window;
    size_t fps;

    bool running;

    thread gui_thread;

    float max_magnitude;
    float spectrogram_length;

    Graph spectrogram_graph;
    Graph spectrogram_derivative_graph;

    void init(size_t _fps, sf::VideoMode _mode, float _max_magnitude, size_t frames_per_buffer, size_t frames_per_fft, PaDeviceIndex device, size_t spectrogram_factor_update_rate,
              function<void(float*)> spectrogram_factor_update_func, Algo _algo);

    void start();
    void stop();
    void wait_for_exit();

    void loop();

    ~GUI();
};
