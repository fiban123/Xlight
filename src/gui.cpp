#include "../include/gui.hpp"

sf::Font font;

void GUI::init(size_t _fps, sf::VideoMode _mode, float _max_magnitude, size_t frames_per_buffer, size_t frames_per_fft, PaDeviceIndex device, size_t spectrogram_factor_update_rate,
               function<void(float*)> spectrogram_factor_update_func, Algo _algo) {

    stream.init(frames_per_buffer, frames_per_fft, device, spectrogram_factor_update_rate, spectrogram_factor_update_func);

    fps = _fps;
    window_mode = _mode;
    max_magnitude = _max_magnitude;
    running = false;
    spectrogram_graph.init({50, 50}, _mode.width - 100, _mode.height - 100, 0, stream.sample_rate / 2, 0, max_magnitude);

    switch (_algo){
        case ALGO_FBDGM:
            algo = make_unique<FBDGM>(stream.sample_ratio, channels, stream.spectrogram);
            break;
        default:
            cout << "Unknown algorithm";
            exit(1);
    }

    algo->init();

    font.loadFromFile("C:/users/Fabia/AppData/local/microsoft/windows/fonts/jetbrainsmono-thin.ttf");

    vector<int> x_marker_values = {0, 100, 1000, 4500, 10000};
    int text_size = 14;
    float marker_len = 20.0f;

    for (size_t i = 0; i < x_marker_values.size(); i++){
        int freq = x_marker_values[i];
        float log_freq = nmap(powf(freq, 0.6f), 0.0f, powf(stream.sample_rate / 2, 0.6f), 0.0f, stream.sample_rate / 2);

        string text = to_string(freq) + " hz";

        spectrogram_graph.x_markers.emplace_back(log_freq, marker_len, text_size, text);
    }

    spectrogram_graph.add_markers();
}

// start the GUI
void GUI::start() {
    cout << "starting GUI" << endl;
    running = true;
    gui_thread = thread(loop, this);
}

// stop the GUI
void GUI::stop() {
    cout << "stopping GUI" << endl;
    running = false;
    gui_thread.join();
}

void GUI::wait_for_exit() {
    while (running) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void GUI::loop() {
    // create the window
    window.create(window_mode, "SFML GUI", sf::Style::Fullscreen);
    window.setFramerateLimit(fps);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }

            else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Escape){
                    window.close();
                }

                else if (event.key.code == sf::Keyboard::W){
                    if (stream.running){
                        stream.stop();
                    }
                    else{
                        stream.start();
                    }
                }

            }
        }

        if (!running){
            window.close();
        }

        algo->execute();

        window.clear(sf::Color(0, 0, 0));

        if (state.draw_spectrogram){
            vector<sf::Vector2f> spectrogram_points(stream.spectrogram.size());
            // calculate graph points
    
            for (size_t i = 0; i < stream.spectrogram.size(); i++){
                float freq = i * stream.sample_ratio;
                float log_freq = nmap(powf(freq, 0.6f), 0.0f, powf(stream.sample_rate / 2, 0.6f), 0.0f, stream.sample_rate / 2);
                spectrogram_points[i] = {log_freq, stream.spectrogram[i]};
            }
            
            spectrogram_graph.add_bars(spectrogram_points, 0.0f);

            spectrogram_graph.draw(window);
        }

        if (state.draw_light_preview) {
            // draw channels

            for (size_t i = 0; i < 4; i++) {
                sf::CircleShape channel;
                channel.setRadius(40.0f);
                channel.setFillColor(channels[i]);
                channel.setPosition(
                    window_mode.width - i * 90.0f - 150.0f,
                    90.0f
                );

                window.draw(channel);
            }

        }

        if (state.draw_info){
            // FFT size, Frames per buffer, sample rate, FFT FPS, unique FFT FPS
            vector<string> settings;

            settings = {
                "Frames per FFT: " + to_string(stream.frames_per_fft),
                "Frames per buffer: " + to_string(stream.frames_per_buffer),
                "sample rate: " + to_string(stream.sample_rate),
                "FFT FPS: " + to_string(stream.sample_rate / stream.frames_per_buffer),
                "unique FFT FPS: " + to_string(stream.sample_rate / stream.frames_per_fft),
            };

            for (size_t i = 0; i < settings.size(); i++){
                string s = settings[i];

                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(14);
                text.setFillColor(sf::Color::White);
                text.setPosition(200.0f, 70 + i * 20);
                text.setString(s);
                window.draw(text);
            }
        }
        

        window.display();
    }
    running = false;

    cout << "GUI stopped" << endl;
}

GUI::~GUI() {
    running = false;
    gui_thread.detach();
}

void Graph::init(sf::Vector2f _pos, float _width, float _height, float _min_x, float _max_x, float _min_y, float _max_y) {
    pos = _pos;
    width = _width;
    height = _height;
    min_x = _min_x;
    max_x = _max_x;
    min_y = _min_y;
    max_y = _max_y;
    
    // initialize bounding vertices
    bounding_vertices = sf::VertexArray(sf::LineStrip, 5);
    bounding_vertices[0] = sf::Vector2f(pos.x, pos.y);
    bounding_vertices[1] = sf::Vector2f(pos.x + width, pos.y);
    bounding_vertices[2] = sf::Vector2f(pos.x + width, pos.y + height);
    bounding_vertices[3] = sf::Vector2f(pos.x, pos.y + height);
    bounding_vertices[4] = sf::Vector2f(pos.x, pos.y);
}

void Graph::add_linear_spline(const vector<sf::Vector2f>& points) {
  vertices = sf::VertexArray(sf::LineStrip, points.size());

  for (size_t i = 0; i < points.size(); i++) {
    vertices[i] = sf::Vector2f{nmap(points[i].x, min_x, max_x, pos.x, pos.x + width), nmap(points[i].y, min_y, max_y, pos.y + height, pos.y)};
    //cout << vertices[i].position.x << " " << vertices[i].position.y << endl;
  }
}

void Graph::add_bars(const vector<sf::Vector2f>& points, float neutral = 0.0f) {
    vertices = sf::VertexArray(sf::Triangles, points.size() * 6);

    float g_neutral = nmap(neutral, min_y, max_y, pos.y + height, pos.y) ;

    for (size_t i = 0; i < points.size() - 1; i++){
        float x = nmap(points[i].x, min_x, max_x, pos.x, pos.x + width);
        float y = nmap(points[i].y + neutral, min_y, max_y, pos.y + height, pos.y);

        float next_x = nmap(points[i + 1].x, min_x, max_x, pos.x, pos.x + width);

        float delta = next_x - x;
        vertices[i * 6] = sf::Vector2f(x, y);
        vertices[i * 6 + 1] = sf::Vector2f(x, g_neutral);
        vertices[i * 6 + 2] = sf::Vector2f(x + delta, y);

        vertices[i * 6 + 3] = sf::Vector2f(x + delta, y);
        vertices[i * 6 + 4] = sf::Vector2f(x + delta, g_neutral);
        vertices[i * 6 + 5] = sf::Vector2f(x, g_neutral);
    }
}

void Graph::add_markers() {
    marker_vertices = sf::VertexArray(sf::Lines, x_markers.size() * 2);


    for (size_t i = 0; i < x_markers.size(); i++) {
        Marker& m = x_markers[i];
        float x = nmap(m.c, min_x, max_x, pos.x, pos.x + width);
        float y = pos.y + height;

        float y2 = y + m.len;


        marker_vertices[i * 2] = sf::Vector2f(x, y);
        marker_vertices[i * 2 + 1] = sf::Vector2f(x, y2);


        m.rtext.setFont(font);
        m.rtext.setCharacterSize(m.text_size);
        m.rtext.setFillColor(sf::Color::White);
        m.rtext.setPosition(x, y2);
        m.rtext.setString(m.text);
    }
}

void Graph::draw(sf::RenderWindow& window) {
    window.draw(bounding_vertices);
    window.draw(marker_vertices);

    for (const Marker& m : x_markers){
        window.draw(m.rtext);
    }

    window.draw(vertices);
}
