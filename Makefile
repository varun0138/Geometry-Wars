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