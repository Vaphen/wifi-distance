#include <gtk/gtk.h>
#include <iostream>
#include "distance.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

struct Point{
	Point() {
		x = 400;
		y = 400;
	}
	int x;
	int y;
};

class DrawingArea {
private:
	GtkWidget *drawArea;
	Point *myPoint;
public:
	DrawingArea() {
		myPoint = new Point;
		drawArea = gtk_drawing_area_new();
		g_signal_connect (drawArea, "draw", G_CALLBACK(&this->draw_cb),  myPoint);
	}

	~DrawingArea() {
		 delete myPoint;
	}

	GtkWidget *getDrawingArea() {
		return drawArea;
	}
private:
	static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {

	   /* Set color for background */
	   cairo_set_source_rgb(cr, 1, 1, 1);
	   /* fill in the background color*/
	   cairo_paint(cr);

	   /* draw grid */
	   cairo_set_source_rgb(cr,0, 0, 0);
	   cairo_set_line_width(cr, 1);

	   for(int i = 100; i < WINDOW_WIDTH; i+=100) {
		   cairo_move_to(cr, 0, i);
		   cairo_line_to(cr, WINDOW_WIDTH, i);
		   cairo_stroke(cr);
	   }
	   for(int i = 100; i < WINDOW_HEIGHT; i+=100) {
	  	   cairo_move_to(cr, i, 0);
	  	   cairo_line_to(cr, i, WINDOW_HEIGHT);
	  	   cairo_stroke(cr);
	   }


	   /* set router properties */
	   cairo_set_source_rgb(cr, 0, 0, 0.8);
	   cairo_set_line_width(cr,6);

	   /* draw 4 routers */
	   cairo_rectangle (cr, 0, 0, 100, 100);
	   cairo_fill(cr);

	   cairo_rectangle (cr, 700, 700, 100, 100);
	   cairo_fill(cr);

	   cairo_rectangle (cr, 0, 700, 100, 100);
	   cairo_fill(cr);

	   cairo_rectangle (cr, 700, 0, 100, 100);
	   cairo_fill(cr);



	   /* draw person point */
	   cairo_set_source_rgb(cr, 0.8, 0, 0);

	   Point *myPoint = (Point*) data;
	   NetworkInformation netInfos;
	   myPoint->x = netInfos.getSignalstrength() * -5;
	   cairo_arc(cr, myPoint->x, myPoint->y, 10, 0, 2*G_PI);
	   cairo_fill(cr);

	   gtk_widget_queue_draw(widget);

	   return FALSE;
	}
};




int main (int argc, char *argv[])
{
   GtkWidget *window;



   gtk_init (&argc, &argv);

   window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
   gtk_window_set_title(GTK_WINDOW(window), "omg");
   gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT);
   gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

   DrawingArea drawArea;

   gtk_container_add(GTK_CONTAINER(window), drawArea.getDrawingArea());
   gtk_widget_show(drawArea.getDrawingArea());
   gtk_widget_show (window);

   gtk_main();

   return 0;
}
