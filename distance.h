#include <iostream>
#include <memory>
#include <cstdio>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <linux/wireless.h>

#define IW_INTERFACE "wlp0s26u1u1"
extern int errno; // used for errno message

class NetworkInformation {
private:
	std::string essid;
	int signalStrength;


	struct iwreq wreq;
	int sockfd;



public:
	NetworkInformation();
	~NetworkInformation();

	bool isSocketOpen();

	std::string getESSID();

	int getSignalstrength();
private:

	/* saves Data about available networks in data-section of struct iwreq */
	bool intern_fillWreqStructWithNetworksSuccess(iwreq *wreq);

	bool intern_scanNetworksSuccess();

	bool intern_initializeESSIDSuccessfull();

	bool intern_initializeSignalstrengthSuccessfull();
	
	bool intern_openSocket();



};
