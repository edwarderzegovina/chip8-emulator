#pragma once
#include <array>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>
//arhitectura pe 8 biti
//4092 biti de memorie
//16 registrii
//opcode e pe 2 bytes + 2 bytes pentru operanzi --> 4 bytes x instructiune.

/*
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/

class Chip8{
    const size_t instructionsInFrame{};
    static constexpr size_t START_ADRESS = 0x200;
    static constexpr size_t FONTSET_SIZE = 80;
    static constexpr size_t FONTSET_START_ADRESS = 0x50;

    static constexpr uint8_t fontset[FONTSET_SIZE] =
    {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };


    std::array<uint8_t,4096> memmory{};
    uint16_t I {0};
    uint16_t PC = {0x200};
    
    std::array<uint8_t, 16> registers{}; 

    std::array<uint16_t,16> stack{};
    uint8_t stackPointer {0};
    
    uint8_t delayTimer {0};
    uint8_t soundTimer{0};

    std::array<uint8_t,32 * 64> video{};

    std::array<uint8_t,16> keys{};

    uint16_t opcode{};

    std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;
    void fetchInstruction(void);
    void nextInstruction(void);

    uint8_t getxRegister(const uint16_t) const;
    uint8_t getyRegister(const uint16_t) const;

    //instructions functions
    /*
    Clear the display.
    */
    void CLS(void);     //00E0
    
    /*
    The interpreter sets the program counter to the address at the top of the stack,
    then subtracts 1 from the stack pointer.
    */
   void RET(void);      //00EE
    
    /*
    The interpreter sets the program counter to nnn
    */
   void JPaddr(void);   //1nnn

    /*
    The interpreter increments the stack pointer, then puts the current PC on the top
    of the stack. The PC is then set to nnn.
    */
    void CALLadr(void);     //2nnn
    /*
    The interpreter compares register Vx to kk, and if they are equal,
    increments the program counter by 2.
    */
    void SEVxByte(void);    //3xkk

    /*
    The interpreter compares register Vx to kk, and if they are not equal,
    increments the program counter by 2.
    */

    void SNEVxByte(void);    //4xkk

    /*
    The interpreter compares register Vx to register Vy, and if they are equal,
    increments the program counter by 2.
    */

    void SEVxVy(void);      //5xy0

    /*
    The interpreter puts the value kk into register Vx.
    */

    void LDVxByte(void);    //6xkk

    /*
    Adds the value kk to the value of register Vx, then stores the result in Vx.
    */
    void ADDVxByte(void);    //7xkk

    /*
    Stores the value of register Vy in register Vx.
    */
   void LDVxVy(void);       //8xy0

    /*
    Set Vx = Vx OR Vy
    */
    void ORVxVy(void);      //8xy1
    void ANDVxVy(void);     //8xy2
    void XORVxVy(void);     //8xy3
    void ADDVxVy(void);     //8xy4
    void SUBVxVy(void);     //8xy5
    void SHRVx(void);       //8xy6
    void SUBNVxVy(void);    //8xy7
    void SHLVx(void);       //8xy8

    /*
    The values of Vx and Vy are compared, and if they are not equal, the
    program counter is increased by 2.
    */
    void SNEVxVy(void);      //9xy0
    void LDIaddr(void);      //Annn
    /*
    The program counter is set to nnn plus the value of V0.
    */
    void JPV0addr(void);     //Bnnn
    /*
    Set Vx = random byte AND kk.
    */
    void RNDVxByte(void);    //Cxkk

    /*
    Dxyn - DRW Vx, Vy, nibble
    Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. The interpreter reads n
    bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen
    at coordinates (Vx, Vy). Sprites are XOR’d onto the existing screen. If this causes any pixels to be erased,
    VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of
    the display, it wraps around to the opposite side of the screen.
    */
    void DRWVxVyN(void);    //Dxyn
    void SKPVx(void);       //Ex9E
    void SKNPVx(void);      //ExA1
    void LDVxDT(void);      //Fx07
    void LDVxK(void);       //Fx0A
    void LDDTVx(void);      //Fx15
    void LDSTVx(void);      //Fx18
    void ADDIVx(void);      //Fx1E
    void LDFVx(void);       //Fx29
    void LDBVx(void);       //Fx33
    void LDIVx(void);       //Fx55
    void LDVxI(void);       //Fx65
    

public:
    void LoadROM(const char* fileName);
    void testRead(void) const;
    Chip8(const size_t = 10);
    const std::array<uint8_t,32*64>& getVideoBuffer() const;
    void setKey(const uint8_t, const bool);
    void updateTimers(void);
    void fetchCycle(void);
};
