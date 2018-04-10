#ifndef STATS_H
#define STATS_H

class Stats {
public:
    Stats();
    Stats(const Stats& st);
    double avg_wait(const bool& qx);
    double avg_srvc(const bool& qx);
    double avg_blkd(const bool& qx);
    double q1_wait; // total wait time of queue1
    double q2_wait; // total wait time of queue2
    double q1_srvc_t; // total service time of queue1
    double q2_srvc_t; // total service time of queue2
    double q1_blkd; // total pkts blocked in queue1
    double q2_blkd; // total pkts blocked in queue2
    double q1_total; // total pkts attempted to be sent to queue1
    double q2_total; // total pkts attempted to be sent to queue1
private:
};

#endif
