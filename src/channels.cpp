inline void CColor::shift_hue(float hue_shift) {
    float _r = (float) r / 255.0f;
    float _g = (float) g / 255.0f;
    float _b = (float) b / 255.0f;

    const float cmax = max(_r, max(_g, _b));
    const float cmin = min(_r, min(_g, _b));

    const float delta = cmax - cmin;

    float h, s, v;
    if (delta == 0.0f){
        h = 0.0f;
    } else if (cmax == _r){
        h = 60.0f * fmod((_g - _b) / delta, 6.0f);
    } else if (cmax == _g){
        h = 60.0f * ((_b - _r) / delta + 2);
    } else{
        h = 60.0f * ((_r - _g) / delta + 4);
    }

    if (cmax != 0){
        s = delta / cmax;
    } else{
        s = 0.0f;
    }

    v = cmax;

    h += hue_shift;
    
    const float c = v * s;
    const float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    const float m = v - c;

    if (h >= 0.0f && h < 60.0f){
        _r = c;
        _g = x;
        _b = 0.0f;
    } else if (h >= 60.0f && h < 120.0f){
        _r = x;
        _g = c;
        _b = 0.0f;
    } else if (h >= 120.0f && h < 180.0f){
        _r = 0.0f;
        _g = c;
        _b = x;
    } else if (h >= 180.0f && h < 240.0f){
        _r = 0.0f;
        _g = x;
        _b = c;
    } else if (h >= 240.0f && h < 300.0f){
        _r = x;
        _g = 0.0f;
        _b = c;
    } else if (h >= 300.0f && h < 360.0f){
        _r = c;
        _g = 0.0f;
        _b = x;
    }


    r = (uint8_t)clamp((_r + m) * 255.0f, 0.0f, 255.0f);
    g = (uint8_t)clamp((_g + m) * 255.0f, 0.0f, 255.0f);
    b = (uint8_t)clamp((_b + m) * 255.0f, 0.0f, 255.0f);
}

CColor::CColor(uint8_t _r, uint8_t _g, uint8_t _b) {
  r = _r;
  g = _g;
  b = _b;
}

void SpectrumAlgo::execute(array<Channel, N_CHANNELS>* base_channels, array<Channel, N_CHANNELS>* transformed_channels, vector<float>* magnitudes) {
    base_algo(base_channels, magnitudes);
    transform_algo(base_channels, transformed_channels, magnitudes);
}

void init_algo() {
    algo = make_unique<ALGO>();
}
