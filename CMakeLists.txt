
cmake_minimum_required(VERSION 3.20)
project(Chip8Emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Găsește Raylib dacă e instalat în sistem / prin Homebrew
find_package(raylib REQUIRED)

file(GLOB SOURCES 
    "src/*.cpp")

add_executable(chip8 ${SOURCES}) # Adaugă aici și restul fișierelor .cpp

# Linkează librăria Raylib
target_link_libraries(chip8 PRIVATE raylib)
