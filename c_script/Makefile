all: process_pictures calc_rgb
process_pictures: process_pictures.c
	g++ -o process_pictures calc_rgb.c `pkg-config --cflags opencv` `pkg-config --libs opencv`
calc_rgb: calc_rgb.c process_pictures.c
	g++ -o calc_rgb calc_rgb.c process_pictures.c `pkg-config --cflags opencv` `pkg-config --libs opencv`
