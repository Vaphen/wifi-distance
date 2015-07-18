#include "distance.h"


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
		std::cerr << "Error description is: " << std::strerror(errno) << std::endl;
		exit(1);
	}

	if(!intern_initializeESSIDSuccessfull()) {
		std::cerr << "Get ESSID ioctl failed." << std::endl;
		std::cerr << "errno = " << errno;
		std::cerr << "Error description : " << strerror(errno) << std::endl;
		exit(2);
	}

	if(!intern_initializeSignalstrengthSuccessfull()) {
		std::cerr << "Get SignalLevel ioctl failed." << std::endl;
		std::cerr << "errno = " << errno;
		std::cerr << "Error description : " << strerror(errno) << std::endl;
		exit(3);
	}

	if(!intern_scanNetworksSuccess()) {
		std::cerr << "Get SignalLevel ioctl failed." << std::endl;
		std::cerr << "errno = " << errno;
		std::cerr << "Error description : " << strerror(errno) << std::endl;
		exit(4);
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




/* saves Data about available networks in data-section of struct iwreq */
bool NetworkInformation::intern_fillWreqStructWithNetworksSuccess(iwreq *wreq) {
	return (ioctl(sockfd, SIOCSIWSCAN, wreq) != 1);
}

bool NetworkInformation::intern_scanNetworksSuccess() {
	wreq = iwreq();
	strncpy(wreq.ifr_ifrn.ifrn_name, IW_INTERFACE, IFNAMSIZ);

	if(!intern_fillWreqStructWithNetworksSuccess(&wreq))
		return false;

	char buf[0xffff]; /* u32 */

	wreq.u.data.pointer = buf;
	wreq.u.data.length = sizeof(buf);
	wreq.u.data.flags = 0;

	ioctl(sockfd, SIOCGIWSCAN, &wreq);
	struct iw_event *iwe = new iw_event;
	memcpy((char*)iwe, buf, IW_EV_LCP_PK_LEN);
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
	if(essidSuccess) {
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

	if(signalstrengthSuccess) {
		this->signalStrength = static_cast<iw_statistics*>(wreq.u.data.pointer)->qual.level - 256;
	}

	/* delete iw_statistics object (cast needed to delete void-pointer) */
	delete static_cast<iw_statistics*>(wreq.u.data.pointer);
	return signalstrengthSuccess;
}

bool NetworkInformation::intern_openSocket() {
	return ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1);
}




