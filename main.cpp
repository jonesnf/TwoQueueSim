#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <chrono>
#include <cmath>
#include "packets.h"

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

void queue_pkt() {
	std::cout << "Add pkt to queue " << std::endl;
}

double service_pkt() {
	std::default_random_engine generator(reseed());
	std::exponential_distribution<double> edist(5);
	return  edist(generator);  
}

// Going to have to return the prev packet for next batch of pkts
void send_pkts(const int& pkts, const int& sec, double& srvc_t, \
                                            Pktq& q1, int& blkd) {
	double start = 0.0;
	double avg_ia = 0.0; // DEBUG
    Packet prev_pkt;
	for ( int i=0; i < pkts; i++ ) {
		std::default_random_engine generator(reseed());
		// might need to change this to exponential? 
		std::uniform_real_distribution<double> urd(sec+start, sec+1.0);
		Packet pkt;	
		pkt.arrv_time = urd(generator);
        pkt.wait_time = 0;
		//std::cout << "	- Packet " << i+1
		//	  << " arrived at: " << pkt.arrv_time
		//	  << std::endl; 
		avg_ia += pkt.arrv_time - (sec+start); // DEBUG
		start = pkt.arrv_time - sec;
		if (SYS_EMPTY) {
		     pkt.srvc_time = service_pkt();
		     pkt.wait_time = 0;
		     srvc_t += pkt.srvc_time;
		     SYS_EMPTY = false;
        } else if ((prev_pkt.srvc_time + prev_pkt.arrv_time) \
                  < pkt.arrv_time) {
             pkt.srvc_time = service_pkt();
		     pkt.wait_time = 0;
		     srvc_t += pkt.srvc_time;
		} else if ((prev_pkt.srvc_time + prev_pkt.arrv_time \
                   + prev_pkt.wait_time)  > pkt.arrv_time) {
		     pkt.srvc_time = service_pkt();
		     pkt.wait_time = prev_pkt.arrv_time + prev_pkt.srvc_time \
                              + prev_pkt.wait_time - pkt.arrv_time;
		     srvc_t += pkt.srvc_time;
		     q1.push(pkt); 
		} else {
		     blkd++;
		}
        prev_pkt = pkt;
	}
//	std::cout << "	- Avg IA: " << avg_ia / pkts << std::endl;
}

void start_sim(const int& lambda) {
	int num_pkts = 0; int sec = 0; int blkd = 0; 
	double srvc_t = 0.0; // total service time
	Pktq q1, q2;
	std::default_random_engine generator(reseed());
	std::poisson_distribution<int> pdist(lambda);
	while ( num_pkts < 100 ) {
		int pkts_arr = pdist(generator);	
		std::cout << pkts_arr  
			  << " packets just arrived"
			  << std::endl; 
		send_pkts(pkts_arr, sec, srvc_t, q1, blkd);
		sec++;
		num_pkts += pkts_arr;
	}	
	std::cout << "Average service time: " 
		  << srvc_t / num_pkts << std::endl;
	std::cout << "Packets in q1: " << q1.size() << std::endl;
	std::cout << "Packets blocked: " << blkd << std::endl;
    std::cout << "Total Packets: " << num_pkts << std::endl;
}


int main(int argc, char* argv[]) {
	double lambda = 0;
	if (argc < 2) {
		std::cerr << "Please enter arrival rate (lambda)" << std::endl;
		return -1;
	} else {
		lambda = atof(argv[1]);
	} 
	start_sim(lambda);	
	return 0;
}
