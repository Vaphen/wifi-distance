#ifndef _DRAWING_AREA_H
#define _DRAWING_AREA_H

#include <gtk/gtk.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "distance.h"

struct Point{
	int x = 400;
	int y = 400;
};

static int average = 0;
static NetworkInformation netInfos;

class DrawingArea {
private:
	GtkWidget *drawArea;
	Point *personLocation;
	std::vector<networkInfo> infoArray;
public:
	DrawingArea();

	~DrawingArea();

	GtkWidget *getDrawingArea();
private:

	static gboolean intern_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);

};

#endif
