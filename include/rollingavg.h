#ifndef ROLLINGAVG_H
#define ROLLINGAVG_H

#include "globals.h"

class RollingAvg
{
public:
    RollingAvg(u8 nSample);
    ~RollingAvg();

    void clear(void);
    void addValue(f32 value);
    f32 getAverage(void);

private:
    u8 _size;
    u8 _count;
    u8 _idx;
    float _sum;
    float *_array;
};

#endif /* ROLLINGAVG_H */