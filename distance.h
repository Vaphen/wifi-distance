#ifndef _DISTANCE_H_
#define _DISTANCE_H_

#include <iostream>
#include <memory>
#include <cstdio>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <iwlib.h>
#include <vector>

enum ERROR { GET_ESSID, GET_SIGNAL_STRENGTH, SCAN_NETWORK, READ_SCAN_RESULTS };

struct networkInfo {
		networkInfo() {
			essid="";
			mac = "";
			level = 0;
		}
		std::string essid;
		std::string mac;
		int level;
	};


#define IW_INTERFACE "wlp0s26u1u1"
extern int errno; // used for errno message

class NetworkInformation {
private:
	std::string essid;
	int signalStrength;
	std::string apMac;

	iwreq wreq;
	int sockfd;
	std::vector<networkInfo> networkList;

public:
	NetworkInformation();
	~NetworkInformation();

	bool isSocketOpen();

	std::string getESSID();
	std::vector<networkInfo> getNetworkList();
	int getSignalstrength();

	void scanNetworks();

private:

	/* saves Data about available networks in data-section of struct iwreq */

	bool intern_fillWreqStructWithNetworksSuccess();

	bool intern_scanNetworksSuccess();

	bool intern_initializeESSIDSuccessfull();

	bool intern_initializeSignalstrengthSuccessfull();
	
	bool intern_openSocket();
	
	bool intern_getAPMacSuccess();

	void intern_updateNetworkList();



};

#endif
