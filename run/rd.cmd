g++ ../src/main.cpp -o xlight_d.exe -static ^
-lsfml-graphics-s -lsfml-window-s -lsfml-system-s ^
-lfftw3f -lportaudio -lftdi1 -lusb-1.0 ^
-lgraphite2 -lusp10 -lgdi32 -lopengl32 -lws2_32 -lfreetype -lpng -lz -lbz2 -lbrotlidec -lbrotlicommon ^
-pthread -lole32 -lsetupapi -lwinmm ^
-m64 -static-libgcc -static-libstdc++ -std=c++23 ^
-Wall -Wextra -Wl,-Bdynamic -lharfbuzz -luuid -ggdb

gdb xlight_d.exe