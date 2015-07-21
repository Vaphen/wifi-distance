#include <gtk/gtk.h>
#include <iostream>
#include <thread>
#include "distance.h"
#include "gui.h"
#include "drawingArea.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800


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

   /* caluclate APs */
    std::vector<networkInfo> infoArray;
	  std::thread t1([&]() {
		  NetworkInformation netInfos;
		  while(1) {
			  try {
				   netInfos.scanNetworks();
				 }catch(ERROR &e) {
				   switch(e) {
						   case ERROR::SCAN_NETWORK: {
							   std::cerr << "Networks could not be scanned." << std::endl;
							   std::cerr << "Try to rerun with root privileges!" << std::endl;
							   std::cerr << "errno = " << errno << std::endl;
							   std::cerr << "Error description : " << strerror(errno) << std::endl;
							   exit(e);
						   }
						   case ERROR::READ_SCAN_RESULTS: {
							   std::cerr << "Reading scanned data failed." << std::endl;
							   std::cerr << "errno = " << errno << std::endl;
							   std::cerr << "Error description : " << strerror(errno) << std::endl;
							   exit(e);
						   }
					   }
				 }
			  infoArray = netInfos.getNetworkList();


		  }

	  });
	  t1.detach();

   gtk_container_add(GTK_CONTAINER(window), drawArea.getDrawingArea());
   gtk_widget_show(drawArea.getDrawingArea());
   gtk_widget_show (window);

   gtk_main();

   return 0;
}
