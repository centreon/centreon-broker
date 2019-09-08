/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NEB_STATISTICS_COMPUTE_VALUE_HH
#define CCB_NEB_STATISTICS_COMPUTE_VALUE_HH

#include <limits>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
namespace statistics {
/**
 *  @class compute_value compute_value.hh
 * "com/centreon/broker/neb/statistics/compute_value.hh"
 *  @brief
 */
template <typename T>
class compute_value {
 public:
  compute_value()
      : _max(std::numeric_limits<T>::min()),
        _min(std::numeric_limits<T>::max()),
        _size(0),
        _sum(0) {}
  compute_value(compute_value const& right) { operator=(right); }
  ~compute_value() {}
  compute_value& operator=(compute_value const& right) {
    if (this != &right) {
      _max = right._max;
      _min = right._min;
      _size = right._size;
      _sum = right._sum;
    }
    return (*this);
  }
  compute_value& operator<<(T value) {
    if (value < _min)
      _min = value;
    if (value > _max)
      _max = value;
    _sum += value;
    ++_size;
    return (*this);
  }
  T avg() const throw() { return (_size ? _sum / _size : 0); }
  T max() const throw() { return (_max); }
  T min() const throw() { return (_min); }
  unsigned int size() const throw() { return (_size); }

 private:
  T _max;
  T _min;
  unsigned int _size;
  T _sum;
};
}  // namespace statistics
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_STATISTICS_COMPUTE_VALUE_HH
