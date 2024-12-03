# Checkers Game

## Description
A simple checkers game implemented in C++ using OpenGL for rendering.

## Prerequisites
- CMake (version 3.10 or higher)
- A C++ compiler that supports C++17
- OpenGL
- GLEW
- GLFW
- GLM (header-only library)

## Installation
To install the required libraries, you can use the following commands:

### On Ubuntu
```bash
sudo apt update
sudo apt install cmake g++ libglew-dev libglfw3-dev libglm-dev
```

### On macOS
```bash
brew install cmake glew glfw glm
```

## Building the Project
1. Navigate to the project directory:
   ```bash
   cd /home/dev/documents/UnP/checker-game-a3
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   make
   ```

## Running the Project
After building the project, you can run the game with the following command:
```bash
./checkers
```
