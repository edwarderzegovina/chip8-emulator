#include <iostream>
#include "machine.hpp"
#include "raylib.h"

using namespace std;
Chip8 chip8(10);

void drawFromVideoBuffer(const Chip8& machine);

int main(){
    chip8.LoadROM("../games/br8kout.ch8");
    const int scalingCoeff = 20;
    const int height = 32;
    const int width = 64;
    const int FPS = 60;
    InitWindow(width * scalingCoeff, height * scalingCoeff, "CHIP-8 Display Test");
    SetTargetFPS(FPS);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        /*
            10 instructions x frame.
        */

        chip8.fetchCycle();

        drawFromVideoBuffer(chip8);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void drawFromVideoBuffer(const Chip8& machine){
    const std::array<uint8_t,32*64>& videoBuffer = machine.getVideoBuffer();
    for(size_t x{};x < 64; x++){
        for(size_t y{};y< 32; y++){
            if(videoBuffer[y*64+x])
                DrawRectangle(x*10,y*10,10,10,WHITE);
        }
    }
}