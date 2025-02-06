g++ ../src/xlight.cpp -o xlight.exe -Wl,-Bstatic -l:libportaudio.a -lfftw3f -lraylib -lopengl32 -lgdi32 -lwinmm -pthread -Wall -static-libgcc -static-libstdc++ -std=c++23 -m64 -lole32 -ldsound -static -lm -lsetupapi

.\xlight