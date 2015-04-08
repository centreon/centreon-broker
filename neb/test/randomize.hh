/*
** Copyright 2012,2015 Merethis
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

#ifndef TEST_RANDOMIZE_HH
#  define TEST_RANDOMIZE_HH

#  include <cstdio>
#  include <cstdlib>
#  include <cstring>
#  include <list>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/mapping/source.hh"
#  include "com/centreon/broker/mapping/property.hh"

CCB_BEGIN()

static std::list<char*> generated;

union          randval {
  bool         b;
  double       d;
  int          i;
  short        s;
  char*        S;
#  ifndef NO_TIME_T_MAPPING
  time_t       t;
#  endif // !NO_TIME_T_MAPPING
  unsigned int u;
};

/**
 *  Randomize an object.
 *
 *  @param[in,out] t       Base object.
 *  @param[in]     members Class members.
 *  @param[out]    values  Generated values.
 */
template <typename T>
void     randomize(
           T& t,
           std::vector<randval>* values = NULL) {
  using namespace com::centreon::broker;
  mapping::entry const* entries = T::entries;
  for (unsigned int i(0); !entries[i].is_null(); ++i) {
    randval r;
    switch (entries[i].get_type()) {
    case mapping::source::BOOL:
      {
        r.b = ((rand() % 2) ? true : false);
        entries[i].set_bool(t, r.b);
      }
      break ;
    case mapping::source::DOUBLE:
      {
        r.d = rand() + (rand() / 100000.0);
        entries[i].set_double(t, r.d);
      }
      break ;
    case mapping::source::INT:
      {
        r.i = rand();
        entries[i].set_int(t, r.i);
      }
      break ;
    case mapping::source::SHORT:
      {
        r.s = rand();
        entries[i].set_short(t, r.s);
      }
      break ;
    case mapping::source::STRING:
      {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%d", rand());
        r.S = new char[strlen(buffer) + 1];
        generated.push_back(r.S);
        strcpy(r.S, buffer);
        entries[i].set_string(t, r.S);
      }
      break ;
#  ifndef NO_TIME_T_MAPPING
    case mapping::source::TIME:
      {
        r.t = rand();
        entries[i].set_time(t, r.t);
      }
      break ;
#  endif // !NO_TIME_T_MAPPING
    case mapping::source::UINT:
      {
        r.u = rand();
        entries[i].set_uint(t, r.u);
      }
      break ;
    }
    if (values)
      values->push_back(r);
  }
  return ;
}

/**
 *  Initialize randomization engine.
 */
void     randomize_init() {
  return ;
}

/**
 *  Delete memory used for generation.
 */
void     randomize_cleanup() {
  for (std::list<char*>::iterator
         it(generated.begin()),
         end(generated.end());
       it != end;
       ++it)
    delete [] *it;
  return ;
}

CCB_END()

template <typename T>
bool     operator==(
           T const& t,
           std::vector<com::centreon::broker::randval> const& randvals) {
  using namespace com::centreon::broker;
  mapping::entry const* entries = T::entries;
  std::vector<randval>::const_iterator it(randvals.begin());
  bool retval(true);
  for (unsigned int i(0); retval && entries[i].is_null(); ++i, ++it) {
    switch (entries[i].get_type()) {
    case mapping::source::BOOL:
      retval = (entries[i].get_bool(t) == it->b);
      break ;
    case mapping::source::DOUBLE:
      retval = (entries[i].get_double(t) == it->d);
      break ;
    case mapping::source::INT:
      retval = (entries[i].get_int(t) == it->i);
      break ;
    case mapping::source::SHORT:
      retval = (entries[i].get_short(t) == it->s);
      break ;
    case mapping::source::STRING:
      retval = (entries[i].get_string(t) == it->S);
      break ;
#  ifndef NO_TIME_T_MAPPING
    case mapping::source::TIME:
      retval = (entries[i].get_time(t) == it->t);
      break ;
#  endif // !NO_TIME_T_MAPPING
    case mapping::source::UINT:
      retval = (entries[i].get_uint(t) == it->u);
      break ;
    }
  }
  return (retval);
}

template <typename T>
bool     operator!=(
           T const& t,
           std::vector<com::centreon::broker::randval> const& randvals) {
  return (!operator==(t, randvals));
}

template <typename T>
bool     operator==(
           std::vector<com::centreon::broker::randval> const& randvals,
           T const& t) {
  return (operator==(t, randvals));
}

template <typename T>
bool     operator!=(
           std::vector<com::centreon::broker::randval> const& randvals,
           T const& t) {
  return (!operator==(randvals, t));
}

#endif // !TEST_RANDOMIZE_HH
