#include "packets.h"

Packet::Packet() {}

Packet::Packet(const double& arr_time) { arrv_time = arr_time; }

Packet::Packet(const Packet& pkt) : arrv_time(pkt.arrv_time), 
	srvc_time(pkt.srvc_time), wait_time(pkt.wait_time) {}
