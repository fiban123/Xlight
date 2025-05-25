import numpy as np
import scipy
from scipy.io.wavfile import write

# Parameters
sample_rate = 44100      # Samples per second
duration = 100           # Duration in seconds
frequency = 1000         # Frequency of sine wave (Hz)

# Time array
t = np.arange(0, duration, 1 / sample_rate)

# Generate sine wave with peak amplitude ±1.0
sine_wave = np.sin(2 * np.pi * frequency * t)

# Scale to 16-bit PCM range: ±32767
audio_data = np.int16(sine_wave * 32767)

# Save as WAV file
write("sine_1000hz_max_volume.wav", sample_rate, audio_data)

print("Generated 'sine_1000hz_max_volume.wav' with 1000 Hz at peak amplitude.")