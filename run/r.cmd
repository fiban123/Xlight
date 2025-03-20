g++ ../src/main.cpp -o xlight.exe ^
-lsfml-graphics-s -lsfml-window-s -lsfml-system-s ^
-lfftw3f -lportaudio -lftdi1 -lusb-1.0 ^
-lopengl32 -lfreetype -lgdi32 -lws2_32 ^
-pthread -lole32 -lsetupapi -lwinmm ^
-static -m64 -static-libgcc -static-libstdc++ -std=c++23 ^
-Wall -Wextra

call xlight.exe