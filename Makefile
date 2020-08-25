CC=gcc
C_FLAGS=-Werror\
	-Wall\
	-Wunused\
	-Wpedantic\
	-g
L_FLAGS= `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

C_FILES=$(wildcard src/*.c)
O_FILES=$(C_FILES:src/%.c=build/%.o)



all: game


run: game
	./$^


game: $(O_FILES)
	$(CC) -o $@ $^ $(L_FLAGS)


build:
	@mkdir -p build


build/%.o: src/%.c | build
	$(CC) -c $< -o $@ $(C_FLAGS)


clean:
	rm -rf game build/*
