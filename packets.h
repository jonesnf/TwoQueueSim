#ifndef PACKETS_H
#define PACKETS_H


class Packet {
public: 
    Packet();
    Packet(const double& arr_time);
    Packet(const Packet& pkt);
    double total_pkt_t();
    double arrv_time; // arrival time of pkt
    double srvc_time; // service time of pkt
    double wait_time; // wait time of pkt
    bool queue1; // send to queue1 by default 
private:
};

#endif
