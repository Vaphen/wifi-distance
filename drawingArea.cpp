#include <gtk/gtk.h>
#include <iostream>
#include <chrono>
#include "distance.h"
#include "drawingArea.h"


DrawingArea::DrawingArea() : 
		personLocation(new Point),
		drawArea(gtk_drawing_area_new()) {
	
	g_signal_connect (drawArea, "draw", G_CALLBACK(&this->intern_draw_cb),  personLocation);

	/* initialize average */
	average = netInfos.getSignalstrength();
}

DrawingArea::~DrawingArea() {
	 delete personLocation;
}

GtkWidget *DrawingArea::getDrawingArea() {
	return drawArea;
}

gboolean DrawingArea::intern_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {

	int areaWidth = gtk_widget_get_allocated_width (widget);
	int areaHeight = gtk_widget_get_allocated_height (widget);
	
   /* Set white background */
   cairo_set_source_rgb(cr, 1, 1, 1);
   cairo_paint(cr);

   /* draw grid */
   cairo_set_source_rgb(cr,0, 0, 0);
   cairo_set_line_width(cr, 1);

   for(int i = areaWidth / 8; i < areaWidth; i+=areaWidth / 8) {
	   cairo_move_to(cr, 0, i);
	   cairo_line_to(cr, areaWidth, i);
	   cairo_stroke(cr);
   }
   for(int i = areaHeight / 8; i < areaHeight; i += areaHeight / 8) {
	   cairo_move_to(cr, i, 0);
	   cairo_line_to(cr, i, areaHeight);
	   cairo_stroke(cr);
   }



   /* get AP-signal strength and essid*/
   int curStrength;
   std::string curEssid;
   try {
	   curStrength = netInfos.getSignalstrength();
	   curEssid = netInfos.getESSID();
   }catch(ERROR &e) {
	   switch(e) {
		   case ERROR::GET_ESSID: {
					std::cerr << "Get ESSID ioctl failed." << std::endl;
					std::cerr << "errno = " << errno;
					std::cerr << "Error description : " << strerror(errno) << std::endl;
					exit(e);
				}
		   case ERROR::GET_SIGNAL_STRENGTH: {
					std::cerr << "Get SignalLevel ioctl failed." << std::endl;
					std::cerr << "errno = " << errno;
					std::cerr << "Error description : " << strerror(errno) << std::endl;
					exit(e);
				}
		   default:
					std::cerr << "Unknown error occured." << std::endl;
					std::cerr << "errno = " << errno;
					std::cerr << "Error description : " << strerror(errno) << std::endl;
					exit(e);
	   }
   }

   /* draw person point (animate, if the signal strength changed) */
   cairo_set_source_rgb(cr, 0.8, 0, 0);
   Point *myPoint = (Point*) data;
   if(curStrength * -8 > myPoint->x) {
	   myPoint->x += 1;
   } else {
	   for(int i = 0; i < 3; i++) {
		   std::clock_t startTimer = std::clock();
		   while(1) {
			   if(curStrength < average - 3 || curStrength > average + 3) {
				   average = curStrength;
			   }
			   if(std::chrono::duration<double, std::milli>(std::clock()-startTimer).count() >= 2000)
				   break;
		   }

	   }
	   myPoint->x = average * -8;
   }
   cairo_arc(cr, myPoint->x, myPoint->y, 10, 0, 2*G_PI);
   cairo_fill(cr);


   /* show current dbm (signalstrength) */
   cairo_set_source_rgb(cr, 0, 0, 0);
   cairo_set_font_size(cr, 18);

   cairo_move_to(cr, 15, areaHeight - 50);
   cairo_show_text(cr, std::string("Connected to: ").append(curEssid).c_str());
   cairo_move_to(cr, 15, areaHeight - 30);
   cairo_show_text(cr, std::to_string(curStrength).append(" dBm").c_str());

   gtk_widget_queue_draw(widget);

   return FALSE;
}