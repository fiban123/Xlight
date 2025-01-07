inline unsigned int get_bin(float freq){
    return freq / sample_ratio;
}

constexpr inline float nmap(float value, float start1, float stop1, float start2, float stop2){
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

inline float nthroot(float x, float root){
    return powf(x, 1.0f / root);
}

inline float normalize_frequency(float freq){
    return nthroot(freq, 1.5f);
}

inline float denormalize_frequency(float nfreq){
    return powf(nfreq, 1.5f);
}

void init_gui(){
    actual_start_freq = start_bin_index * sample_ratio;
    actual_end_freq = end_bin_index * sample_ratio;
    
    max_normalized_amplitude = normalize_frequency(actual_end_freq);
    min_normalized_amplitude = normalize_frequency(actual_start_freq);

    max_latency_ms = 1 / (sample_rate / FRAMES_PER_FFT) * 1000;
    FFT_rate = sample_rate / FRAMES_PER_BUFFER;

    graph_points = vector<rl::Vector2>(n_bins + 1);

    top_left = {150.0f, 50.0f};
    bottom_left = {150.0f, SCREEN_HEIGHT - 70.0f};
    top_right = {SCREEN_WIDTH - 50.0f, 50.0f};
    bottom_right = {SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT - 70.0f};
}

void calculate_graph_points(){
    graph_points[0] = {bottom_left.x, bottom_left.y - 20};

    // calculate points
    for (unsigned int i = 0; i < n_bins; i++){


        float magnitude = normalized_magnitudes[i + start_bin_index];

        float frequency = (i + start_bin_index)* sample_ratio;

        float normalized_frequency = normalize_frequency(frequency);

        // map points to coordinates on the screen
        // map amplitude from (0 to MAX_AMPLITUDE) to Y position on screen
        float amplitude_graphpos = nmap(magnitude, 0, MAX_AMPLITUDE, SCREEN_HEIGHT - 70, 50);
        // map frequency from (min_normalized_amplitude to max_normalized_amplitude) to X position on screen
        float frequency_graphpos = nmap(normalized_frequency, min_normalized_amplitude, max_normalized_amplitude, 150, SCREEN_WIDTH - 50);

        rl::Vector2 point;
        point.x = frequency_graphpos;
        point.y = amplitude_graphpos;

        graph_points[i + 1] = point;
    }
}

void draw_channels(){
    float transformed_startx = SCREEN_WIDTH - 100;
    float transformed_starty = 150;

    float base_startx = SCREEN_WIDTH - 100;
    float base_starty = 250;

    float spacing = 100;
    float radius = 40;

    for (unsigned int i = 0; i < N_CHANNELS; i++){
        rl::Color transformed_col;
        transformed_col.r = (uint8_t)transformed_channels[i].col.r;
        transformed_col.g = (uint8_t)transformed_channels[i].col.g;
        transformed_col.b = (uint8_t)transformed_channels[i].col.b;
        transformed_col.a = (uint8_t)255;

        rl::Color base_col;
        base_col.r = (uint8_t)base_channels[i].col.r;
        base_col.g = (uint8_t)base_channels[i].col.g;
        base_col.b = (uint8_t)base_channels[i].col.b;
        base_col.a = (uint8_t)255;


        rl::DrawCircle(transformed_startx - i * spacing, transformed_starty, radius, transformed_col);
        rl::DrawCircle(base_startx - i * spacing, base_starty, radius, base_col);
    }
}

void execute_gui(){
    rl::InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Xlight");
    rl::SetTargetFPS(240);


    while (!rl::WindowShouldClose()){

        if (rl::IsKeyPressed(rl::KEY_F11)){
            rl::ToggleFullscreen();
        }

        rl::BeginDrawing();

        rl::ClearBackground(rl::DARKGRAY);


        // draw graph bounds
        rl::DrawLine(top_left.x, top_left.y, bottom_left.x, bottom_left.y, rl::WHITE);
        rl::DrawLine(bottom_left.x, bottom_left.y, bottom_right.x, bottom_right.y, rl::WHITE);

        // render text
        rl::DrawText("amplitude", 20, SCREEN_HEIGHT / 2, 20, rl::WHITE);
        rl::DrawText("frequency (hz)", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 45, 20, rl::WHITE);

        // draw indicator lines
        rl::DrawLine(bottom_left.x, bottom_left.y, bottom_left.x - 20, bottom_left.y, rl::WHITE);
        rl::DrawLine(top_left.x, top_left.y, top_left.x - 20, top_left.y, rl::WHITE);

        rl::DrawLine(bottom_left.x, bottom_left.y, bottom_left.x, bottom_left.y + 20, rl::WHITE);
        rl::DrawLine(bottom_right.x, bottom_right.y, bottom_right.x, bottom_right.y + 20, rl::WHITE);

        // draw indicator values
        rl::DrawText((to_string((int)actual_start_freq) + " hz").c_str(), bottom_left.x - 8, bottom_left.y + 25, 30, rl::WHITE);
        rl::DrawText((to_string((int)actual_end_freq) + " hz").c_str(), bottom_right.x - 100, bottom_right.y + 25, 30, rl::WHITE);

        rl::DrawText(to_string(MAX_AMPLITUDE).c_str(), top_left.x - 100, top_left.y - 12, 30, rl::WHITE);
        rl::DrawText("0", bottom_left.x - 45, bottom_left.y - 12, 30, rl::WHITE);

        rl::DrawText((to_string(rl::GetFPS()) + " FPS").c_str(), 200, 30, 30, rl::WHITE);
        rl::DrawText(("max latency: " + to_string(max_latency_ms) + " ms").c_str(), 400, 30, 20, rl::WHITE);
        rl::DrawText(("FFT rate: " + to_string(FFT_rate) + " Hz").c_str(), 800, 30, 20, rl::WHITE);

        calculate_graph_points();

        draw_channels();

        // map cursor x from (150 to screen_width) to (FREQ_LOWER_BOUND to FREQ_UPPER_BOUND)
        float cursor_normalized_frequency = nmap((float)rl::GetMouseX(), 150, SCREEN_WIDTH - 50, min_normalized_amplitude, max_normalized_amplitude);
        float cursor_frequency = denormalize_frequency(cursor_normalized_frequency);

        rl::DrawText(to_string(cursor_frequency).c_str(), top_right.x - 200, top_right.y, 40, rl::WHITE);


        // draw graph
        rl::DrawSplineLinear(graph_points.data(), graph_points.size(), 1.0f, rl::RED);

        for (size_t i = 0; i < lines.size(); i++){
                float normalized_frequency = normalize_frequency(lines[i]);
    
                float x = nmap(normalized_frequency, min_normalized_amplitude, max_normalized_amplitude, 150, SCREEN_WIDTH - 50);

                rl::DrawLine(x, 0, x, SCREEN_HEIGHT, rl::BLUE);
        }

        rl::EndDrawing();
    }

    rl::CloseWindow();
}

void stop_gui(){ // stops the GUI
    rl::CloseWindow();
}