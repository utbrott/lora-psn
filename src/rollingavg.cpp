#include "rollingavg.h"

RollingAvg::RollingAvg(u8 n)
{
    _size = n;
    _ar = (float *)malloc(_size * sizeof(f32));
    if (_ar == NULL)
        _size = 0;
    clear();
}

RollingAvg::~RollingAvg()
{
    if (_ar != NULL)
        free(_ar);
}

/**
 * @brief Clears all counters and sum
 */
void RollingAvg::clear()
{
    _cnt = 0;
    _idx = 0;
    _sum = 0;

    // Prealloc with only zeros
    for (u8 i = 0; i < _size; ++i)
        _ar[i] = 0.0;
}

/**
 * @brief Adds value to RA
 * @param value Value to add
 */
void RollingAvg::addValue(f32 value)
{
    if (_ar == NULL)
        return;

    _sum -= _ar[_idx];
    _ar[_idx] = value;
    _sum += _ar[_idx];
    ++_idx;

    // Check if array is filled
    if (_idx == _size)
        _idx = 0;

    if (_cnt < _size)
        ++_cnt;
}

/**
 * @brief Get the average value
 * @return float Rolling average value
 */
float RollingAvg::getAverage()
{
    if (_cnt == 0)
        return NAN;

    return (_sum / _cnt);
}
