void calculate_graph_points();

void execute_gui();

void stop_gui();

inline unsigned int get_bin(float freq);

constexpr inline float nmap(float value, float start1, float stop1, float start2, float stop2);

inline float nthroot(float x, float root);

inline float normalize_frequency(float freq);

inline float denormalize_frequency(float nfreq);

float max_normalized_amplitude;
float min_normalized_amplitude;

float actual_start_freq;
float actual_end_freq;

float FFT_rate;
float max_latency_ms;

// graph bounds
rl::Vector2 top_left;
rl::Vector2 bottom_left;
rl::Vector2 top_right;
rl::Vector2 bottom_right;

vector<rl::Vector2> graph_points;

vector<float> lines;