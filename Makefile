all:
	g++ -o start.bin main.cpp -O0 -lm -lpthread -lX11 -g -lfftw3
clean:
	rm start.bin
