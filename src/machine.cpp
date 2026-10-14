#include "machine.hpp"

Chip8::Chip8(const size_t instructionsOnFrame):randGen(std::chrono::system_clock::now().time_since_epoch().count()),instructionsInFrame(instructionsOnFrame){
    PC = 0x200;
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
    for(size_t i{};i<FONTSET_SIZE;++i){
        memmory[FONTSET_START_ADRESS+i] = fontset[i];
    }
}

void Chip8::LoadROM(const char* fileName){
    std::ifstream file(fileName,std::ios::binary | std::ios::ate);
    if(file.is_open()){
        std::streampos size = file.tellg();
        char* buffer =  new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer,size);
        file.close();
        for(size_t i{};i<size;++i)
            memmory[START_ADRESS+i] = buffer[i];
        delete[] buffer;

    }
}
void Chip8::testRead() const{
    for (size_t i {}; i < 0xFFF; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(memmory[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
        }
}

const std::array<uint8_t,32*64> Chip8::getVideoBuffer() const{
    return video;
}

uint8_t Chip8::getxRegister(const uint16_t block) const{
    return static_cast<uint8_t>((block & 0x0F00) >> 8);
}

uint8_t Chip8::getyRegister(const uint16_t block) const{
    return static_cast<uint8_t>((block & 0x00F0) >> 4);
}

void Chip8::nextInstruction(){
    opcode = (memmory[PC]<<8) | memmory[PC+1];
    PC+=2;
}

void Chip8::fetchInstruction(){
    
}

void Chip8::fetchCycle(){
    for(size_t i{}; i < instructionsInFrame; ++i){
        fetchInstruction();  //Aproved by teoBSN. 
        //todo: fetch asincron la urmatoarea instructiune (ILP)
    }
}
void Chip8::CLS(){
    video.fill(0);
}
void Chip8::RET(){
    PC = stack[--stackPointer];
}

void Chip8::JPaddr(){
    PC = opcode & 0x0FFF;
}

void Chip8::CALLadr(){
    stack[stackPointer++] = PC;
    PC = opcode & (0x0FFF);
}
void Chip8::SEVxByte(){
    uint8_t x = getxRegister(opcode);
    if(registers[x] == (opcode & 0x00FF)){
        PC += 2;
    }
}

void Chip8::SNEVxByte(){
    uint8_t x = getxRegister(opcode);
    if(registers[x] != (opcode & 0x00FF)){
        PC += 2;
    }
}

void Chip8::SEVxVy(){
    uint8_t x = getxRegister(opcode);
    uint8_t y = getyRegister(opcode);
    if(registers[x] == registers[y]){
        PC += 2;
    }
}

void Chip8::LDVxByte(){
    registers[getxRegister(opcode)] = opcode & 0x00FF;
}
void Chip8::ADDVxByte(){
    registers[getxRegister(opcode)] += (opcode & 0x00FF);
}
void Chip8::LDVxVy(){
    registers[getxRegister(opcode)] = registers[getyRegister(opcode)];
}
void Chip8::ORVxVy(){
    registers[getxRegister(opcode)] |= registers[getyRegister(opcode)];
}
void Chip8::ANDVxVy(){
    registers[getxRegister(opcode)] &= registers[getyRegister(opcode)];
}
void Chip8::XORVxVy(){
    registers[getxRegister(opcode)] ^= registers[getyRegister(opcode)];
}
void Chip8::ADDVxVy(){
    uint8_t x = getxRegister(opcode);
    uint8_t y = getyRegister(opcode);
    registers[x] += registers[y];
    registers[0xF] = (registers[x] < registers[y]);
}void Chip8::SUBVxVy(){
    uint8_t x = getxRegister(opcode);
    uint8_t y = getyRegister(opcode);
    bool borrow = (registers[x] >= registers[y])? 1 : 0;
    registers[x] -= registers[y];
    registers[0xF] = static_cast<uint8_t>(borrow);
}
void Chip8::SUBNVxVy(){
    uint8_t x = getxRegister(opcode);
    uint8_t y = getyRegister(opcode);
    bool borrow = (registers[y] >= registers[x]) ? 1 : 0;
    registers[x] = registers[y] - registers[x];
    registers[0xF] = static_cast<uint8_t>(borrow);
}
void Chip8::SHRVx(){
    uint8_t x = getxRegister(opcode);
    bool lsb = registers[x] & 1;
    registers[x] >>= 1;
    registers[0xF] = static_cast<uint8_t>(lsb);
}
void Chip8::SHLVx(){
    uint8_t x = getxRegister(opcode);
    bool msb = registers[x] & 0x90;
    registers[x] <<= 1;
    registers[0xF] = static_cast<uint8_t>(msb);
}
void Chip8::SNEVxVy(){
    if(registers[getxRegister(opcode)] != registers[getyRegister(opcode)])
        PC+=2;
}
void Chip8::LDIaddr(){
    I = opcode & 0x0FFF;
}
void Chip8::JPV0addr(){
    PC = registers[0] + (opcode & 0x0FFF);
}
void Chip8::RNDVxByte(){
    uint8_t rnd = randByte(randGen);
    registers[getxRegister(opcode)] = rnd & (opcode & 0x00FF);
}
