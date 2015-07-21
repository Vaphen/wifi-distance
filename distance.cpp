#include "distance.h"
#include <chrono>
#include <thread>

NetworkInformation::NetworkInformation() {
	/* initialize Variables */
	essid = "";
	signalStrength = 0;
	wreq = iwreq();
	sockfd = -1;

	/* try to open socket */
	if (!this->isSocketOpen() && this->intern_openSocket()) {
		std::cerr << "Cannot open socket" << std::endl;
		std::cerr << "errno = %d" << std::endl;
		std::cerr << "Error description is: " << std::strerror(errno)
				<< std::endl;
		exit(1);
	}

}

NetworkInformation::~NetworkInformation() {
	close(sockfd);
}

bool NetworkInformation::isSocketOpen() {
	return (sockfd != -1);
}

std::string NetworkInformation::getESSID() {
	if (!intern_initializeESSIDSuccessfull())
		throw ERROR::GET_ESSID;
	return this->essid;
}

int NetworkInformation::getSignalstrength() {
	if (!intern_initializeSignalstrengthSuccessfull())
		throw ERROR::GET_SIGNAL_STRENGTH;
	return this->signalStrength;
}

std::vector<networkInfo> NetworkInformation::getNetworkList() {
		return networkList;
}

void NetworkInformation::scanNetworks() {
		if (!intern_fillWreqStructWithNetworksSuccess())
			throw ERROR::SCAN_NETWORK;
		if(!intern_scanNetworksSuccess())
			throw ERROR::READ_SCAN_RESULTS;
}

/* saves Data about available networks in data-section of struct iwreq */
bool NetworkInformation::intern_fillWreqStructWithNetworksSuccess() {
	 wreq = iwreq();
	 strncpy(wreq.ifr_ifrn.ifrn_name, IW_INTERFACE, IFNAMSIZ);
	 int ret = (ioctl(sockfd, SIOCSIWSCAN, &wreq));
	 return (ret != -1);
}

bool NetworkInformation::intern_scanNetworksSuccess() {
	wreq = iwreq();
	networkList.clear();
	strncpy(wreq.ifr_ifrn.ifrn_name, IW_INTERFACE, IFNAMSIZ);


	char buf[0xffff]; /* u32 */



	std::clock_t startTimer = std::clock();
	int counter = 0;
	while(1) {
		wreq.u.data.pointer = buf;
		wreq.u.data.length = sizeof(buf);
		wreq.u.data.flags = 0;
		/* we got no answer in 10 secs, there must be a problem somewhere */
		if(counter == 10)
			return false;
		if(ioctl(sockfd, SIOCGIWSCAN, &wreq) == 0)
			break;
		if((std::clock() - startTimer) / CLOCKS_PER_SEC <= 1)
			continue;
		counter++;
		startTimer = std::clock();
	}

	struct iw_event myEvent;
	struct stream_descr stream;
	struct iw_range range;
	memset(&stream, 0, sizeof(stream));

	if(iw_get_range_info(sockfd, IW_INTERFACE, &range) < 0)
		return false;

	iw_init_event_stream(&stream, (char *) buf, wreq.u.data.length);

	int ret = 0;
	networkInfo *curNetwork = new networkInfo;
	while(iw_extract_event_stream(&stream, &myEvent, range.we_version_compiled) > 0) {

		switch (myEvent.cmd) {
			case SIOCGIWAP:
				//curNetwork.mac = myEvent.u.ap_addr.sa_data;
				// printf("          Network %d:\n", count+1);
				break;
			case SIOCGIWESSID: {
					char essid[IW_ESSID_MAX_SIZE + 1];
					memset(essid, '\0', sizeof(essid));
					if ((myEvent.u.essid.pointer) && (myEvent.u.essid.length))
						memcpy(essid, myEvent.u.essid.pointer, myEvent.u.essid.length);

					(myEvent.u.essid.flags) ? curNetwork->essid = essid : curNetwork->essid = "hidden";
					networkList.push_back(*curNetwork);
					delete curNetwork;
					curNetwork = new networkInfo;
				}

				break;
			case IWEVQUAL: {
				curNetwork->level = (myEvent.u.qual.level == 0) ? 0 : myEvent.u.qual.level - 255;

			}
				break;
			case SIOCGIWENCODE: {
				/*	if(event->u.data.flags & IW_ENCODE_DISABLED)
				 networks[count].encrypted = 0;
				 else
				 networks[count].encrypted = 1;

				 printf("                    Encryption key:%s\n", networks[count].encrypted ? "on" : "off");
				 break;*/
				break;

			}
		}
	}
	delete curNetwork;

	return true;
}

bool NetworkInformation::intern_initializeESSIDSuccessfull() {
	/* initialize wreq structure */
	wreq = iwreq();
	char* id = new char[IW_ESSID_MAX_SIZE + 1];
	wreq.u.essid.pointer = id;
	wreq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
	std::sprintf(wreq.ifr_name, IW_INTERFACE);

	/* initialize structure */
	bool essidSuccess = (ioctl(sockfd, SIOCGIWESSID, &wreq) != -1);

	/* set locale essid variable in case of successfull initialization */
	if (essidSuccess) {
		this->essid = std::string(static_cast<char*>(wreq.u.essid.pointer));
	}

	/* delete Pointer and return success */
	delete[] id;
	return essidSuccess;
}

bool NetworkInformation::intern_initializeSignalstrengthSuccessfull() {
	/* initialize wreq structure */
	wreq = iwreq();
	wreq.u.data.pointer = new iw_statistics;
	wreq.u.data.length = sizeof(iw_statistics);
	std::sprintf(wreq.ifr_name, IW_INTERFACE);

	/* get infos about signalstrength */
	bool signalstrengthSuccess = (ioctl(sockfd, SIOCGIWSTATS, &wreq) != -1);

	if (signalstrengthSuccess) {
		this->signalStrength = static_cast<iw_statistics*>(wreq.u.data.pointer)->qual.level - 256;
	}

	/* delete iw_statistics object (cast needed to delete void-pointer) */
	delete static_cast<iw_statistics*>(wreq.u.data.pointer);
	return signalstrengthSuccess;
}

bool NetworkInformation::intern_openSocket() {
	return ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1);
}

bool NetworkInformation::intern_getAPMacSuccess() {
	wreq = iwreq();
	std::sprintf(wreq.ifr_name, IW_INTERFACE);

	wreq.u.addr.sa_family = AF_INET;

	int i = ioctl(sockfd, SIOCGIWAP, &wreq);

	return (i != -1);
}

