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

bool SYS_PROC = false;

unsigned reseed() {
	return chrono_clk::now().time_since_epoch().count();
}

void queue_pkt() {
	std::cout << "Add pkt to queue " << std::endl;
}

void service_pkt() {
	std::default_random_engine generator(reseed());
	std::exponential_distribution<double> edist(5);
	double srvc_time = edist(generator);  
	std::cout << "pkt service time: " << srvc_time << std::endl;
	//SYS_PROC = true;
}

void send_pkts(const int& pkts, const int& sec) {
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
		(!SYS_PROC) ? service_pkt() : queue_pkt();
	}
	std::cout << "	- Avg IA: " << avg_ia / pkts << std::endl;
}

void start_sim(const int& lambda) {
	int num_pkts = 0; int sec = 0; 
	std::queue<double> q1, q2;
	std::default_random_engine generator(reseed());
	std::poisson_distribution<int> pdist(lambda);
	while ( num_pkts < 30 ) {
		int pkts_arr = pdist(generator);	
		std::cout << pkts_arr  
			  << " packets just arrived"
			  << std::endl; 
		send_pkts(pkts_arr, sec);
		sec++;
		num_pkts += pkts_arr;
	}
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
