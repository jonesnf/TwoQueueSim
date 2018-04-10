#include "stats.h"

Stats::Stats() {
    q1_wait = 0.0;
    q2_wait = 0.0;
    q1_srvc_t = 0.0;
    q2_srvc_t = 0.0;
    q1_blkd = 0;
    q2_blkd = 0;
    q1_total = 0;
    q2_total = 0;    
}

Stats::Stats(const Stats& s) : q1_srvc_t(s.q1_srvc_t), q2_srvc_t(s.q2_srvc_t),
    q1_blkd(s.q1_blkd), q2_blkd(s.q2_blkd), q1_total(s.q1_total),
    q2_total(s.q2_total){}

double Stats::avg_wait(const bool& qx) {
    if (qx) 
        return q1_wait / (q1_total - q1_blkd);
    else 
        return q2_wait / (q2_total - q2_blkd); 
}

double Stats::avg_srvc(const bool& qx) {
    if (qx) 
        return q1_srvc_t / (q1_total - q1_blkd);
    else 
        return q2_srvc_t / (q2_total - q2_blkd); 
}


double Stats::avg_blkd(const bool& qx) {
    if (qx) 
        return q1_blkd / q1_total;
    else 
        return q2_blkd / q2_total; 
}
