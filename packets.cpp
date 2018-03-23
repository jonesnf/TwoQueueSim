#include "packets.h"

Packet::Packet() {
    arrv_time = 0.0;
    srvc_time = 0.0;
    wait_time = 0.0;
}

Packet::Packet(const double& arr_time) { arrv_time = arr_time; }

Packet::Packet(const Packet& pkt) : arrv_time(pkt.arrv_time), 
	srvc_time(pkt.srvc_time), wait_time(pkt.wait_time){}

/*
 * Returns the total time of the packet spent in system + queue
 */
double Packet::total_pkt_t() { return arrv_time + srvc_time + wait_time; }
