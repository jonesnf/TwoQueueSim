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
#include "stats.h"

/*
Copyright (C) 2017 by Nathan Jones (jonesnf@miamioh.edu) 

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
 * Print table of relevant stats. 
 * Stats functions => 1 means queue 1, 0 means queue2
 **/
void print_table(Stats& stats, const Settings& cfg) {
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Arrivals         | " << cfg.num_pkts << std::endl;
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q1 Srvc Time Avg | " << stats.avg_srvc(1) << std::endl;
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q2 Srvc Time Avg | " << stats.avg_srvc(0) << std::endl;
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q1 Waiting   Avg | " << stats.avg_wait(1) << std::endl;
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q2 Waiting   Avg | " << stats.avg_wait(0) << std::endl; 
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q1 Blocking  Avg | " << stats.avg_blkd(1) << std::endl;
    std::cout << std::string(40, '~') << std::endl;
    std::cout << "| Q2 Blocking  Avg | " << stats.avg_blkd(0) << std::endl; 
    std::cout << std::string(40, '~') << std::endl;
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

//TODO: shorten
void send_pkts(Stats& stat, Pktq& q1, Pktq& q2,\
                                            const Settings& cfg) {
    bool sel_q1 = true; // select q1 by default
    Packet ref, prev_pkt1, prev_pkt2;
    for ( int i=0; i < cfg.num_pkts; i++ ) {
        std::default_random_engine generator(reseed());
        std::exponential_distribution<double> ia_time(8);
        Packet pkt;	
        pkt.arrv_time = ia_time(generator) + ref.arrv_time;
        if (q1.front().total_pkt_t() < pkt.arrv_time && !q1.empty()) q1.pop(); 
        if (q2.front().total_pkt_t() < pkt.arrv_time && !q2.empty()) q2.pop(); 
        sel_q1 = sel_queue(cfg); // Select a queue
        if (sel_q1) { 
            if (add_queue(prev_pkt1, pkt, q1, cfg)) {
                stat.q1_srvc_t += pkt.srvc_time;
                stat.q1_wait += pkt.wait_time;
            } else {
                stat.q1_blkd++;
            }
            stat.q1_total++;
            prev_pkt1 = pkt;
        } else {        
            if (add_queue(prev_pkt2, pkt, q2, cfg)) {
                stat.q2_srvc_t += pkt.srvc_time;
                stat.q2_wait += pkt.wait_time;
            } else {
                stat.q2_blkd++;
            }
            stat.q2_total++;
            prev_pkt2 = pkt;
        }
        ref = pkt;
    }
}

void start_sim(const Settings& cfg) {
    Pktq q1, q2;
    Stats stat;
    bool sim = true;
    while ( sim ) {
        send_pkts(stat, q1, q2, cfg);
        sim = false;
    }	
    flush_pkts(q1, stat.q1_srvc_t);
    flush_pkts(q2, stat.q2_srvc_t);
    print_table(stat, cfg);
}


int main(int argc, char* argv[]) {
    Settings cfg;
    if (argc < 6) {
    std::cerr << "Please enter the following: ./<program> <lambda>"
              << " <mu> <queue size> <phi> <num_pkts>"  << std::endl;
    return -1;
    } else {
        cfg.lambda   = atof(argv[1]);
        cfg.mu       = atof(argv[2]);
        cfg.queue_sz = atof(argv[3]);
        cfg.phi      = atof(argv[4]);
        cfg.num_pkts = atof(argv[5]);
    } 
    start_sim(cfg);	
    return 0;
}
