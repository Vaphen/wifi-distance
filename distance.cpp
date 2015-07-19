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

	if (!intern_initializeESSIDSuccessfull()) {
		std::cerr << "Get ESSID ioctl failed." << std::endl;
		std::cerr << "errno = " << errno;
		std::cerr << "Error description : " << strerror(errno) << std::endl;
		exit(2);
	}

	if (!intern_initializeSignalstrengthSuccessfull()) {
		std::cerr << "Get SignalLevel ioctl failed." << std::endl;
		std::cerr << "errno = " << errno;
		std::cerr << "Error description : " << strerror(errno) << std::endl;
		exit(3);
	}

}

NetworkInformation::~NetworkInformation() {
	close(sockfd);
}

bool NetworkInformation::isSocketOpen() {
	return (sockfd != -1);
}

std::string NetworkInformation::getESSID() {
	return this->essid;
}

int NetworkInformation::getSignalstrength() {
	return this->signalStrength;
}

std::vector<networkInfo> NetworkInformation::getNetworkList() {
		return networkList;
}

bool NetworkInformation::performScanNetworksSuccess() {
	return intern_scanNetworksSuccess();
}

/* saves Data about available networks in data-section of struct iwreq */
bool NetworkInformation::intern_fillWreqStructWithNetworksSuccess(iwreq *wreq) {
	 int ret = (ioctl(sockfd, SIOCSIWSCAN, wreq));
	 return (ret != -1);
}

bool NetworkInformation::intern_scanNetworksSuccess() {
	wreq = iwreq();
	strncpy(wreq.ifr_ifrn.ifrn_name, IW_INTERFACE, IFNAMSIZ);

	if (!intern_fillWreqStructWithNetworksSuccess(&wreq))
		return false;


	char buf[0xffff]; /* u32 */

	wreq.u.data.pointer = buf;
	wreq.u.data.length = sizeof(buf);
	wreq.u.data.flags = 0;

	if(ioctl(sockfd, SIOCGIWSCAN, &wreq) < 0) {
		std::cerr << "reading scn results failed" << std::endl;
	}

	struct iw_event myEvent;
	struct stream_descr stream;
	struct iw_range range;
	memset(&stream, 0, sizeof(stream));
	if(iw_get_range_info(sockfd, IW_INTERFACE, &range) < 0) {
		std::cerr << "rangecheck failed" << std::endl;
	}

	iw_init_event_stream(&stream, (char *) buf, wreq.u.data.length);
	int ret = 0;
	for(int i = 0; i < 500; i++) {
		ret = iw_extract_event_stream(&stream, &myEvent, range.we_version_compiled);

		if(ret < 0)
			break;
		networkInfo curNetwork;
		switch (myEvent.cmd) {
		case SIOCGIWAP:
			curNetwork.mac = myEvent.u.ap_addr.sa_data;
			networkList.push_back(curNetwork);
			// printf("          Network %d:\n", count+1);
			break;
		case SIOCGIWESSID: {
				char essid[IW_ESSID_MAX_SIZE + 1];
				memset(essid, '\0', sizeof(essid));
				if ((myEvent.u.essid.pointer) && (myEvent.u.essid.length))
					memcpy(essid, myEvent.u.essid.pointer, myEvent.u.essid.length);

				(myEvent.u.essid.flags) ? networkList.back().essid = essid : networkList.back().essid = "hidden";
			}

			break;
		case IWEVQUAL: {
			//curNetwork.level = static_cast<iw_statistics*>(myEvent.u.data.pointer)->qual.level - 256;
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

