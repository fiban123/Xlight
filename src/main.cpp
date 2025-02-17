#include "../src/xlight.cpp"

#include <portaudio.h>


int main(){
    Xlight xlight;

    xlight.gui.init(240, sf::VideoMode::getDesktopMode());

    xlight.gui.start();

    xlight.gui.wait_for_exit();

    return 0;
}