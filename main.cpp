#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <chrono>

/*
Copyright (C) 2017 by jonesnf@miamioh.edu

File: main.cpp

Author: N8
*/
using chrono_clk = std::chrono::system_clock;

bool SYS_EMPTY = true;

unsigned reseed() {
	return chrono_clk::now().time_since_epoch().count();
}

void queue_pkt() {
	std::cout << "Add pkt to queue " << std::endl;
}

void service_pkt(double& total_srvc_t) {
	std::default_random_engine generator(reseed());
	std::exponential_distribution<double> edist(5);
	double srvc_time = edist(generator);  
	total_srvc_t += srvc_time;
}

void send_pkts(const int& pkts, const int& sec, double& srvc_t) {
	double start = 0.0;
	double avg_ia = 0.0;
	std::queue<double> q1, q2;
	for ( int i=0; i < pkts; i++ ) {
		std::default_random_engine generator(reseed());
		std::uniform_real_distribution<double> urd(sec+start, sec+1.0);
		double pkt_time = urd(generator);
		//std::cout << "	- Packet " << i+1
		//	  << " arrived at: " << pkt_time
		//	  << std::endl; 
		avg_ia += pkt_time - (sec+start);
		start = pkt_time - sec;
		(SYS_EMPTY) ? service_pkt(srvc_t) : queue_pkt();
	}
//	std::cout << "	- Avg IA: " << avg_ia / pkts << std::endl;
}

void start_sim(const int& lambda) {
	int num_pkts = 0; int sec = 0; 
	double total_srvc_t = 0.0;
	std::default_random_engine generator(reseed());
	std::poisson_distribution<int> pdist(lambda);
	while ( num_pkts < 100 ) {
		int pkts_arr = pdist(generator);	
		std::cout << pkts_arr  
			  << " packets just arrived"
			  << std::endl; 
		send_pkts(pkts_arr, sec, total_srvc_t);
		sec++;
		num_pkts += pkts_arr;
	}
	std::cout << "Average service time: " 
		  << total_srvc_t / num_pkts << std::endl;
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
