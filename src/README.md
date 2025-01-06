channels:

format: 
i{x} = index x in the frequency spectrogram.
f{x} = spectrogram index of the frequency x in the spectrogram

channel 1 (bass): i1 + i2 + i3/2
channel 2 (mids): i3/2 + i4 + i4 ... + (i(f5000) - i1) + i(f5000) / 2
channel 3 (highs): i(f5000) / 2 + (i(f5000) + 1) ... i(f22000)
channel 4 (total energy derivative): d/dx (total energy)

total energy = sum of all frequency energies
total energy derivative: rate of positive change in total energy