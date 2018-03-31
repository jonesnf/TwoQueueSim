#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <chrono>
#include <cmath>
#include "packets.h"
#include "settings.h"

/*
Copyright (C) 2017 by jonesnf@miamioh.edu

File: main.cpp

Author: N8
*/
using chrono_clk = std::chrono::system_clock;
using Pktq = std::queue<Packet>;

bool SYS_EMPTY = true;

unsigned reseed() {
    return chrono_clk::now().time_since_epoch().count();
}

/*
 * Flush the rest of the packets in the queue
 */
void flush_pkts(Pktq& pq, double& srvc_t) {
    while (!pq.empty()){
        srvc_t += pq.front().srvc_time;  
        pq.pop(); 
    }
}

/* 
 * Give service time to packet based on exponential distribution
 */
double service_pkt(const Settings& cfg) {
    std::default_random_engine generator(reseed());
    std::exponential_distribution<double> edist(cfg.mu);
    return  edist(generator);  
}

/*
 * Add pkt to queue after selecting which queue to place
 */
bool add_queue(Packet& prev_pkt, Packet& pkt, Pktq& pq, const Settings& cfg) {
        if (SYS_EMPTY) {
            pkt.srvc_time = service_pkt(cfg);
            SYS_EMPTY = false;
        } else if (prev_pkt.total_pkt_t() < pkt.arrv_time && pq.empty()) {
            pkt.srvc_time = service_pkt(cfg);
            pkt.wait_time = 0;
        } else if (prev_pkt.total_pkt_t() > pkt.arrv_time && \
                                    pq.size() < cfg.queue_sz) {
            pkt.srvc_time = service_pkt(cfg);
            pkt.wait_time = prev_pkt.total_pkt_t() - pkt.arrv_time;
            pq.push(pkt);
        } else if (prev_pkt.total_pkt_t() < pkt.arrv_time && \
                                    pq.size() < cfg.queue_sz) {
            pkt.srvc_time = service_pkt(cfg);
            pkt.wait_time = 0;
            pq.push(pkt);
        } else {
            // Packet was blocked
            return false; 
        }
    return true;
}

/* 
 * Select which queue (system) to process packet
 */
bool sel_queue(const Settings& cfg) {
    std::default_random_engine generator(reseed());
    std::uniform_real_distribution<double> queue_sel(0.0, 1.0);
    return (queue_sel(generator) > 1-cfg.phi) ? true : false; 
}

// TODO: Make function shorter
void send_pkts(const int& pkts, const int& sec, double& srvc_t, \
               Packet& prev_pkt, Pktq& q1, Pktq& q2, const Settings& cfg,\
                                                                 int& blkd) {
    double start = 0.0; bool sel_q1 = true; // select q1 by default
    double avg_ia = 0.0; // DEBUG
    for ( int i=0; i < pkts; i++ ) {
        std::default_random_engine generator(reseed());
        // TODO: Change this to exponential 
        std::uniform_real_distribution<double> urd(sec+start, sec+1.0);
        Packet pkt;	
        pkt.arrv_time = urd(generator);
        if (q1.front().total_pkt_t() < pkt.arrv_time && !q1.empty()) q1.pop(); 
        if (q2.front().total_pkt_t() < pkt.arrv_time && !q2.empty()) q2.pop(); 
        //std::cout << "	- Packet " << i+1
        //	  << " arrived at: " << pkt.arrv_time
        //	  << std::endl; 
        avg_ia += pkt.arrv_time - (sec+start); // DEBUG
        start = pkt.arrv_time - sec;
        sel_q1 = sel_queue(cfg); // Select a queue
        if (sel_q1) 
         (add_queue(prev_pkt, pkt, q1, cfg)) ? srvc_t += pkt.srvc_time : blkd++;
        else        
         (add_queue(prev_pkt, pkt, q2, cfg)) ? srvc_t += pkt.srvc_time : blkd++;
        prev_pkt = pkt;
    }
}

void start_sim(const Settings& cfg) {
    int num_pkts = 0; int sec = 0; int blkd = 0; 
    double srvc_t = 0.0; // total service time
    Pktq q1, q2;
    Packet prev_pkt;
    std::default_random_engine generator(reseed());
    std::poisson_distribution<int> pdist(cfg.lambda);
    while ( num_pkts < 6000 ) {
        int pkts_arr = pdist(generator);	
        send_pkts(pkts_arr, sec, srvc_t, prev_pkt, q1, q2, cfg, blkd);
        sec++;
        num_pkts += pkts_arr;
    }	
    flush_pkts(q1, srvc_t);
    flush_pkts(q2, srvc_t);
    std::cout << "Average service time: " << srvc_t / (num_pkts-blkd) << std::endl;
    std::cout << "Packets blocked: " << blkd << std::endl;
    std::cout << "Total Packets: " << num_pkts << std::endl;
}


int main(int argc, char* argv[]) {
    Settings cfg;
    if (argc < 5) {
    std::cerr << "Please enter the following: ./<program> <lambda>"
              << " <mu> <queue size> <phi>"  << std::endl;
    return -1;
    } else {
        cfg.lambda   = atof(argv[1]);
        cfg.mu       = atof(argv[2]);
        cfg.queue_sz = atof(argv[3]);
        cfg.phi      = atof(argv[4]);
    } 
    start_sim(cfg);	
    return 0;
}
