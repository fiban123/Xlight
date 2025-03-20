#include "../include/serial.hpp"

FT232R::FT232R() {
    cout << "initializing FTDI serrial communication" << endl;

    ctx = ftdi_new();
    
    assert(ctx); 

    int ret = ftdi_usb_open(ctx, 0x0403, 0x6001);
    if (ret < 0) {
        string err = ftdi_get_error_string(ctx);
        ftdi_free(ctx);
        throw runtime_error("Unable to open FTDI device: " + err);
    }

    ret = ftdi_set_baudrate(ctx, 250000);
    if (ret < 0) {
        string err = ftdi_get_error_string(ctx);
        ftdi_usb_close(ctx);
        ftdi_free(ctx);
        throw runtime_error("Unable to set baud rate: " + err);
    }


    // Set line properties: 8 data bits, 2 stop bits, no parity.
    ret = ftdi_set_line_property(ctx, BITS_8, STOP_BIT_2, (ftdi_parity_type)0);
    if (ret < 0) {
        string err = ftdi_get_error_string(ctx);
        ftdi_usb_close(ctx);
        ftdi_free(ctx);
        throw runtime_error("Unable to set line properties: " + err);
    }
}

inline void FT232R::busy_wait(const unsigned long int ns) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::nanoseconds(ns);
    while (std::chrono::high_resolution_clock::now() < end) {
        // Busy-wait
    }
}

inline int FT232R::set_break(ftdi_break_type state) {
    return ftdi_set_line_property2(ctx, BITS_8, STOP_BIT_2, (ftdi_parity_type)0, state);
}

void FT232R::write_dmx_frame(vector<unsigned char> data) {
    int ret;

    // --- Break ---
    ret = set_break(BREAK_ON);
    if (ret < 0) {
        cerr << "Failed to set break on: " << ftdi_get_error_string(ctx) << endl;
        return;
    }
    busy_wait(100 * 1000); // wait 100us

    // --- Mark After Break ---
    ret = set_break(BREAK_OFF);
    if (ret < 0) {
        cerr << "Failed to set break off: " << ftdi_get_error_string(ctx) << endl;
        return;
    }
    busy_wait(12 * 1000); // wait 12us

    // --- Frame Body ---
    // Prepend the DMX start code (0x00) to the data.
    std::vector<unsigned char> frame;
    frame.push_back(0x00);  // DMX start code
    frame.insert(frame.end(), data.begin(), data.end());

    ret = ftdi_write_data(ctx, frame.data(), static_cast<int>(frame.size()));
    if (ret < 0) {
        std::cerr << "Failed to write DMX data: " << ftdi_get_error_string(ctx) << std::endl;
    }
    // --- Idle ---
    busy_wait(1 * 1000000); // wait for 10 ms
}

FT232R::~FT232R() {
    if (ctx) {
        std::cout << "Closing FTDI device." << std::endl;
        // Optionally reset or cancel transfers here
        int ret = ftdi_usb_close(ctx);
        if (ret < 0) {
            std::cerr << "ftdi_usb_close error: " << ftdi_get_error_string(ctx) << std::endl;
        }
        ftdi_free(ctx);
        ctx = nullptr;
    }
}

void DMXInterface::start() {
    running = true;
    serial_thread = thread(loop, this);
}

void DMXInterface::stop() {
    running = false;
    serial_thread.join();
}

void DMXInterface::loop() {
    while (running){
        vector<unsigned char> frame(4 * 4);

        frame[0 * 4] = channels->at(0).r;
        frame[0 * 4 + 1] = channels->at(0).g;
        frame[0 * 4 + 2] = channels->at(0).b;
        frame[0 * 4 + 3] = max(frame[0*4], max(frame[0*4 + 1], frame[0*4+2]));

        for (int i = 1; i < 3; i++){
            frame[i * 4] = channels->at(i).r;
            frame[i * 4 + 1] = channels->at(i).g;
            frame[i * 4 + 2] = channels->at(i).b;
            frame[i * 4 + 3] = clamp((frame[i * 4] + frame[i * 4 + 1] + frame[i * 4 + 2]) / 3.0f, 0.0f, 255.0f);
            //frame[i * 4 + 3] = 0;
        }
        frame[3 * 4] = channels->at(3).r;
        frame[3 * 4 + 1] = channels->at(3).g;
        frame[3 * 4 + 2] = channels->at(3).b;
        frame[3 * 4 + 3] = clamp(frame[3*4] + frame[3*4 + 1] + frame[3*4 + 2], 0, 255);
        cout << endl;

        //frame = {255, 0, 0, 0,   0, 255, 0, 0,   0, 0, 255, 0,   0, 0, 0, 255};

        for (int i = 0; i < 4*4; i++){
            cout << setw(3) << (int) frame[i] << " ";
        }

        //frame = {0, 0, 0, 0};
        serial_ifc.write_dmx_frame(frame);
    }
}

DMXInterface::~DMXInterface() {
    if (running){
        running = false;
        serial_thread.detach();
    }
}
