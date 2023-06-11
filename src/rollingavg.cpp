#include "rollingavg.h"

RollingAvg::RollingAvg(u8 nSample)
{
  _size = nSample;
  _array = (float *)malloc(_size * sizeof(f32));
  if (_array == NULL)
    _size = 0;
  clear();
}

RollingAvg::~RollingAvg()
{
  if (_array != NULL)
    free(_array);
}

/**
 * @brief Clears all counters and sum
 */
void RollingAvg::clear()
{
  _count = 0;
  _idx = 0;
  _sum = 0;

  // Prealloc with only zeros
  for (u8 i = 0; i < _size; ++i)
    _array[i] = 0.0;
}

/**
 * @brief Adds value to RA
 * @param value Value to add
 */
void RollingAvg::addValue(f32 value)
{
  if (_array == NULL)
    return;

  _sum -= _array[_idx];
  _array[_idx] = value;
  _sum += _array[_idx];
  ++_idx;

  if (_idx == _size) // Set _idx to 0 if max reached
    _idx = 0;

  if (_count < _size)
    ++_count;
}

/**
 * @brief Get the average value
 * @return float Rolling average value
 */
float RollingAvg::getAverage()
{
  if (_count == 0)
    return NAN;

  return (_sum / _count);
}
