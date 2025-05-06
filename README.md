# Geometry Wars
This game is a simplified Geometry Wars-inspired shooter where the player battles enemies in a confined space. The player can move, shoot, and use a special ability while facing off against multiple enemy types. The game is controlled using keyboard and mouse input, with an option to pause gameplay.

## Game Controls
- **Movement**: WASD for player movement
- **Fire Weapon**: Left mouse button
- **Special Ability**: Right mouse button - Fires a unique weapon
- **Pause**: P key

## Demo
![Game](Resources/Gifs/demo.gif)

## Building the Project
**Note**: These instructions are specifically for Windows. For other platforms, adjust the paths and compilation steps accordingly.

### Requirements

- C++17 or higher
- SFML 2.6.1 or higher (prebuilt binaries)

#### Setting up SFML
1. **Clone the project**:
   - Clone the repository from GitHub:
   ```bash
   git clone https://github.com/varun0138/Geometry-Wars
   cd Geometry-Wars
   ```

2. **Download SFML**: 
   - Download prebuilt binaries for SFML from the official [SFML website](https://www.sfml-dev.org/index.php).
   
3. **Configure your project**:
   - Extract the SFML binaries and place them in a folder (e.g., SFML-2.6.1).
   
4. **Add SFML to your environment**:
   - Update the `Makefile` included in the project with SFML libs and include paths.
   
   Example `Makefile`:
   ```makefile
   all: compile link run clean

   SFML_INCLUDE = C:/SFML-2.6.1/include
   SFML_LIB = C:/SFML-2.6.1/lib

   compile:
      g++ -m64 -std=c++17 -Wall -Wextra -Wpedantic -O3 -I$(SFML_INCLUDE) -DSFML_STATIC -c src/*.cpp 

   link:
      g++ *.o -o main -L $(SFML_LIB) -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s -lfreetype -lvorbisenc -lvorbisfile -lvorbis -logg -lFLAC -lopenal32 -lopengl32 -lwinmm -lgdi32

   clean:
      del *.o *.exe
      
   run:
      ./main
   ```
5. **Compile the project**:
   ```
   make
   ```