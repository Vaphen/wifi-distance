all:
	g++ -liw -std=c++11 -Wextra -g -o cairo1 `pkg-config --cflags --libs gtk+-3.0` -lgtk-3 omain.cpp distance.cpp distance.h
