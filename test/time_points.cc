/*
** Copyright 2015 Centreon
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

#include "test/time_points.hh"

using namespace com::centreon::broker::test;

/**
 *  Default constructor.
 */
time_points::time_points() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
time_points::time_points(time_points const& other) : _points(other._points) {}

/**
 *  Destructor.
 */
time_points::~time_points() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
time_points& time_points::operator=(time_points const& other) {
  if (this != &other)
    _points = other._points;
  return (*this);
}

/**
 *  Get point at some index.
 *
 *  @param[in] index  Point index.
 *
 *  @return Time point of the index.
 */
time_t time_points::operator[](int index) const {
  return (_points[index]);
}

/**
 *  Get last point.
 *
 *  @return Last point.
 */
time_t time_points::last() const {
  return (_points[_points.size() - 1]);
}

/**
 *  Get point previous last point.
 *
 *  @return Point previous last point.
 */
time_t time_points::prelast() const {
  return (_points[_points.size() - 2]);
}

/**
 *  Store a new point.
 */
void time_points::store() {
  _points.push_back(time(NULL));
  return;
}
