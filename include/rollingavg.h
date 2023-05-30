#ifndef ROLLINGAVG_H
#define ROLLINGAVG_H

#include "globals.h"

class RollingAvg
{
public:
    RollingAvg(u8);
    ~RollingAvg();

    void clear(void);
    void addValue(f32);
    f32 getAverage(void);

private:
    u8 _size;
    u8 _cnt;
    u8 _idx;
    float _sum;
    float *_ar;
};

#endif /* ROLLINGAVG_H */