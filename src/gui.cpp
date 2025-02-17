#include "../include/gui.hpp"


// initialize the GUI object
void GUI::init(size_t _fps, sf::VideoMode _mode){
    fps = _fps;
    mode = _mode;
    running = false;
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
    window.create(mode, "SFML GUI");
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
            }
        }

        if (!running){
            window.close();
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
