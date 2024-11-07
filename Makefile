windows:
	gcc -I include -L lib -o main src/main.c src/window/windowManager.c src/input/input.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lSDL2_ttf

mac:
	gcc -I include -L lib -o main src/main.c src/window/windowManager.c src/input/input.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
