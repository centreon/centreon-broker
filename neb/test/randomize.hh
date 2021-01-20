/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#ifndef TEST_RANDOMIZE_HH
#define TEST_RANDOMIZE_HH

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <vector>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mapping/property.hh"
#include "com/centreon/broker/mapping/source.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"

CCB_BEGIN()

union randval {
  bool b;
  double d;
  int i;
  short s;
  char* S;
  time_t t;
  uint32_t u;
};

void randomize(io::data& t, std::vector<randval>* values = NULL);
void randomize_init();
void randomize_cleanup();

CCB_END()

template <typename T>
bool operator==(T const& t,
                std::vector<com::centreon::broker::randval> const& randvals) {
  using namespace com::centreon::broker;
  mapping::entry const* entries = T::entries;
  std::vector<randval>::const_iterator it(randvals.begin());
  bool retval(true);
  for (uint32_t i(0); retval && entries[i].is_null(); ++i, ++it) {
    switch (entries[i].get_type()) {
      case mapping::source::BOOL:
        retval = (entries[i].get_bool(t) == it->b);
        break;
      case mapping::source::DOUBLE:
        retval = (entries[i].get_double(t) == it->d);
        break;
      case mapping::source::INT:
        retval = (entries[i].get_int(t) == it->i);
        break;
      case mapping::source::SHORT:
        retval = (entries[i].get_short(t) == it->s);
        break;
      case mapping::source::STRING:
        retval = (entries[i].get_string(t) == it->S);
        break;
      case mapping::source::TIME:
        retval = (entries[i].get_time(t) == it->t);
        break;
      case mapping::source::UINT:
        retval = (entries[i].get_uint(t) == it->u);
        break;
    }
  }
  return (retval);
}

template <typename T>
bool operator!=(T const& t,
                std::vector<com::centreon::broker::randval> const& randvals) {
  return (!operator==(t, randvals));
}

template <typename T>
bool operator==(std::vector<com::centreon::broker::randval> const& randvals,
                T const& t) {
  return (operator==(t, randvals));
}

template <typename T>
bool operator!=(std::vector<com::centreon::broker::randval> const& randvals,
                T const& t) {
  return (!operator==(randvals, t));
}
#endif  // !TEST_RANDOMIZE_HH
