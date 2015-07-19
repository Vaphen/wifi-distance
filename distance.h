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

enum ERROR { SCAN_NETWORK_ERROR };

struct networkInfo {
		networkInfo() {
			essid="";
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

	bool performScanNetworksSuccess();
	bool isSocketOpen();

	std::string getESSID();
	std::vector<networkInfo> getNetworkList();
	int getSignalstrength();

private:


	/* saves Data about available networks in data-section of struct iwreq */
	bool intern_fillWreqStructWithNetworksSuccess(iwreq *wreq);

	bool intern_scanNetworksSuccess();

	bool intern_initializeESSIDSuccessfull();

	bool intern_initializeSignalstrengthSuccessfull();
	
	bool intern_openSocket();
	
	bool intern_getAPMacSuccess();



};
