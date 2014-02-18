/*
** Copyright 2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NEB_STATISTICS_COMPUTE_VALUE_HH
#  define CCB_NEB_STATISTICS_COMPUTE_VALUE_HH

#  include <limits>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            neb {
  namespace          statistics {
    /**
     *  @class compute_value compute_value.hh "com/centreon/broker/neb/statistics/compute_value.hh"
     *  @brief
     */
    template<typename T>
    class            compute_value {
    public:
                     compute_value()
        : _max(std::numeric_limits<T>::min()),
          _min(std::numeric_limits<T>::max()),
          _size(0),
          _sum(0) {}
                     compute_value(compute_value const& right) {
        operator=(right);
      }
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
      T              avg() const throw () {
        return (_size ? _sum / _size : 0);
      }
      T              max() const throw () { return (_max); }
      T              min() const throw () { return (_min); }
      unsigned int   size() const throw () { return (_size); }

    private:
      T              _max;
      T              _min;
      unsigned int   _size;
      T              _sum;
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_COMPUTE_VALUE_HH
