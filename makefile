all:
	g++ -liw -std=c++11 -Wextra -g -o cairo1 `pkg-config --cflags --libs gtk+-3.0` -lgtk-3 gui.cpp gui.h distance.cpp distance.h drawingArea.cpp drawingArea.h graph.cpp graph.h
