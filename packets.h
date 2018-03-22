#ifndef PACKETS_H
#define PACKETS_H


class Packet {
public: 
	Packet();
	Packet(const double& arr_time);
	Packet(const Packet& pkt);
	double arrv_time;
	double srvc_time;
	double wait_time;
private:
};

#endif
