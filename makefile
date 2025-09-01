all:
	g++ digit.cpp -o digit `pkg-config --cflags --libs opencv4`
