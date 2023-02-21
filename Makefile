all: build

build:
	gcc Tema.c -o movies

clean:
	rm ./movies
