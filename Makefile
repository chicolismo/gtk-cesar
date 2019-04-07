main:
	#gcc -o main main.c simulator.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -o main main.c simulator.c -Wall `pkg-config --cflags --libs gtk+-3.0`

clean:
	rm main
