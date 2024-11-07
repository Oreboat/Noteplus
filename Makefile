windows:
	gcc -I include -L lib -o main src/main.c src/window/windowManager.c src/input/input.c -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lSDL2_ttf

mac_real:
	gcc -I include -I /opt/homebrew/opt/sdl2/include -I /opt/homebrew/opt/sdl2_ttf/include \
	-L /opt/homebrew/opt/sdl2/lib -L /opt/homebrew/opt/sdl2_ttf/lib \
    -o main src/main.c src/window/windowManager.c src/input/input.c \
    -lSDL2main -lSDL2 -lSDL2_ttf

