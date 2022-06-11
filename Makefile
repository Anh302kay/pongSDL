release:
	g++ -O3 -g -std=c++20 -Wall source/*.cpp -I include -I res -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lzip -mwindows -o bin/release/game.exe
debug:
	g++ -g -std=c++20 -Wall source/*.cpp -I include -I res -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lzip -mwindows -static-libstdc++ -o bin/debug/game.exe

web:
	em++ -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -s SDL2_IMAGE_FORMATS='["png"]' -std=c++20 -Iinclude -Ires source/main.cpp source/window_draw.cpp source/window_events.cpp source/renderwindow.cpp source/entity.cpp --preload-file .\res -o b.html --use-preload-plugins -mnontrapping-fptoint