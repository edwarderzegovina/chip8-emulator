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
        const std::streamsize size = file.tellg();
        if(size < 0 || static_cast<size_t>(size) > memmory.size() - START_ADRESS)
            return;
        char* buffer = new char[static_cast<size_t>(size)];
        file.seekg(0, std::ios::beg);
        file.read(buffer,size);
        file.close();
        for(std::streamsize i{};i<size;++i)
            memmory[START_ADRESS + static_cast<size_t>(i)] =
                static_cast<uint8_t>(buffer[i]);
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

const std::array<uint8_t,32*64>& Chip8::getVideoBuffer() const{
    return video;
}

void Chip8::setKey(const uint8_t key, const bool pressed){
    if(key < keys.size())
        keys[key] = static_cast<uint8_t>(pressed);
}

void Chip8::updateTimers(){
    if(delayTimer > 0)
        --delayTimer;
    if(soundTimer > 0)
        --soundTimer;
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
    nextInstruction();
    switch(opcode & 0xF000){
        case 0x0000:
            if(opcode == 0x00E0) CLS();
            else if(opcode == 0x00EE) RET();
            break;
        case 0x1000: JPaddr(); break;
        case 0x2000: CALLadr(); break;
        case 0x3000: SEVxByte(); break;
        case 0x4000: SNEVxByte(); break;
        case 0x5000: if((opcode & 0x000F) == 0) SEVxVy(); break;
        case 0x6000: LDVxByte(); break;
        case 0x7000: ADDVxByte(); break;
        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0: LDVxVy(); break;
                case 0x1: ORVxVy(); break;
                case 0x2: ANDVxVy(); break;
                case 0x3: XORVxVy(); break;
                case 0x4: ADDVxVy(); break;
                case 0x5: SUBVxVy(); break;
                case 0x6: SHRVx(); break;
                case 0x7: SUBNVxVy(); break;
                case 0xE: SHLVx(); break;
                default: break;
            }
            break;
        case 0x9000: if((opcode & 0x000F) == 0) SNEVxVy(); break;
        case 0xA000: LDIaddr(); break;
        case 0xB000: JPV0addr(); break;
        case 0xC000: RNDVxByte(); break;
        case 0xD000: DRWVxVyN(); break;
        case 0xE000:
            if((opcode & 0x00FF) == 0x9E) SKPVx();
            else if((opcode & 0x00FF) == 0xA1) SKNPVx();
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x07: LDVxDT(); break;
                case 0x0A: LDVxK(); break;
                case 0x15: LDDTVx(); break;
                case 0x18: LDSTVx(); break;
                case 0x1E: ADDIVx(); break;
                case 0x29: LDFVx(); break;
                case 0x33: LDBVx(); break;
                case 0x55: LDIVx(); break;
                case 0x65: LDVxI(); break;
                default: break;
            }
            break;
        default: break;
    }
}

void Chip8::fetchCycle(){
    for(size_t i{}; i < instructionsInFrame; ++i){
        fetchInstruction();  //Aproved by teoBSN. 
    }
    updateTimers();
}
void Chip8::CLS(){
    video.fill(0);
}
void Chip8::RET(){
    if(stackPointer > 0)
        PC = stack[--stackPointer];
}

void Chip8::JPaddr(){
    PC = opcode & 0x0FFF;
}

void Chip8::CALLadr(){
    if(stackPointer >= stack.size())
        return;
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
    const uint16_t result = registers[x] + registers[y];
    registers[0xF] = static_cast<uint8_t>(result > 0xFF);
    registers[x] = static_cast<uint8_t>(result);
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
    bool borrow = (registers[y] <= registers[x]) ? 1 : 0;
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
    bool msb = registers[x] & 0x80;
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

void Chip8::DRWVxVyN(){
    const uint8_t x = registers[getxRegister(opcode)] % 64;
    const uint8_t y = registers[getyRegister(opcode)] % 32;
    const uint8_t height = opcode & 0x000F;
    registers[0xF] = 0;
    for(uint8_t row{}; row < height; ++row){
        const uint8_t sprite = memmory[I + row];
        for(uint8_t bit{}; bit < 8; ++bit){
            if((sprite & (0x80 >> bit)) == 0)
                continue;
            const size_t pixel = ((y + row) % 32) * 64 + ((x + bit) % 64);
            if(video[pixel])
                registers[0xF] = 1;
            video[pixel] ^= 1;
        }
    }
}

void Chip8::SKPVx(){
    if(keys[registers[getxRegister(opcode)] & 0x0F])
        PC += 2;
}

void Chip8::SKNPVx(){
    if(!keys[registers[getxRegister(opcode)] & 0x0F])
        PC += 2;
}

void Chip8::LDVxDT(){ registers[getxRegister(opcode)] = delayTimer; }

void Chip8::LDVxK(){
    for(uint8_t key{}; key < keys.size(); ++key){
        if(keys[key]){
            registers[getxRegister(opcode)] = key;
            return;
        }
    }
    PC -= 2;
}

void Chip8::LDDTVx(){ delayTimer = registers[getxRegister(opcode)]; }
void Chip8::LDSTVx(){ soundTimer = registers[getxRegister(opcode)]; }
void Chip8::ADDIVx(){ I += registers[getxRegister(opcode)]; }
void Chip8::LDFVx(){ I = FONTSET_START_ADRESS + 5 * (registers[getxRegister(opcode)] & 0x0F); }

void Chip8::LDBVx(){
    const uint8_t value = registers[getxRegister(opcode)];
    memmory[I] = value / 100;
    memmory[I + 1] = (value / 10) % 10;
    memmory[I + 2] = value % 10;
}

void Chip8::LDIVx(){
    const uint8_t x = getxRegister(opcode);
    for(uint8_t i{}; i <= x; ++i)
        memmory[I + i] = registers[i];
}

void Chip8::LDVxI(){
    const uint8_t x = getxRegister(opcode);
    for(uint8_t i{}; i <= x; ++i)
        registers[i] = memmory[I + i];
}
