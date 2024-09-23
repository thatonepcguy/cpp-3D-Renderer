A 3D renderer I built using c++ and SDL2

I compiled this on debian using libsdl2-dev so if you want to compile it on windows you will have to take extra steps
Eventually I plan to include precompiled binarys for both windows and linux

im using this to compile on linux: g++ -o main main.cpp objParser.cpp objectRasterizer.cpp objCulling.cpp lighting.cpp -lSDL2 -std=c++2b