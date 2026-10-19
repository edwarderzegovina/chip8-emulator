#include <iostream>
#include "machine.hpp"
#include "raylib.h"

Chip8 chip8(10);

void drawFromVideoBuffer(const Chip8& machine, const int scalingCoeff);

int main(){
    chip8.LoadROM("../games/Tic-Tac-Toe [David Winter].ch8");
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

        for(int key{}; key < 16; ++key)
            chip8.setKey(static_cast<uint8_t>(key), false);
        const int keyMap[16] = {
            KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
            KEY_Q, KEY_W, KEY_E, KEY_A,
            KEY_S, KEY_D, KEY_Z, KEY_C,
            KEY_FOUR, KEY_R, KEY_F, KEY_V
        };
        for(int key{}; key < 16; ++key)
            chip8.setKey(static_cast<uint8_t>(key), IsKeyDown(keyMap[key]));

        chip8.fetchCycle();

        drawFromVideoBuffer(chip8, scalingCoeff);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void drawFromVideoBuffer(const Chip8& machine, const int scalingCoeff){
    const std::array<uint8_t,32*64>& videoBuffer = machine.getVideoBuffer();
    for(size_t x{};x < 64; x++){
        for(size_t y{};y< 32; y++){
            if(videoBuffer[y*64+x])
                DrawRectangle(x*scalingCoeff,y*scalingCoeff,
                              scalingCoeff,scalingCoeff,WHITE);
        }
    }
}