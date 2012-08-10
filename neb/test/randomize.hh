/*
** Copyright 2012 Merethis
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
#  include "mapping.hh"

CCB_BEGIN()

static std::list<char*> generated;

union          randval {
  bool         b;
  double       d;
  int          i;
  short        s;
  char*        S;
  time_t       t;
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
  mapped_data<T> const* members(mapped_type<T>::members);
  for (unsigned int i(0); members[i].type; ++i) {
    randval r;
    switch (members[i].type) {
    case 'b':
      if (i && (members[i - 1].member.b == members[i].member.b))
        continue ;
      {
        r.b = ((rand() % 2) ? true : false);
        (t.*members[i].member.b) = r.b;
      }
      break ;
    case 'd':
      if (i && (members[i - 1].member.d == members[i].member.d))
        continue ;
      {
        r.d = rand() + (rand() / 100000.0);
        (t.*members[i].member.d) = r.d;
      }
      break ;
    case 'i':
      if (i && (members[i - 1].member.i == members[i].member.i))
        continue ;
      {
        r.i = rand();
        (t.*members[i].member.i) = r.i;
      }
      break ;
    case 's':
      if (i && (members[i - 1].member.s == members[i].member.s))
        continue ;
      {
        r.s = rand();
        (t.*members[i].member.s) = r.s;
      }
      break ;
    case 'S':
      if (i && (members[i - 1].member.S == members[i].member.S))
        continue ;
      {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%d", rand());
        r.S = new char[strlen(buffer) + 1];
        generated.push_back(r.S);
        strcpy(r.S, buffer);
        (t.*members[i].member.S) = r.S;
      }
      break ;
    case 't':
      if (i && (members[i - 1].member.t == members[i].member.t))
        continue ;
      {
        r.t = rand();
        (t.*members[i].member.t) = r.t;
      }
      break ;
    case 'u':
      if (i && (members[i - 1].member.u == members[i].member.u))
        continue ;
      {
        r.u = rand();
        (t.*members[i].member.u) = r.u;
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
  mapped_data<T> const* members(mapped_type<T>::members);
  std::vector<randval>::const_iterator it(randvals.begin());
  bool retval(true);
  for (unsigned int i(0); retval && members[i].type; ++i, ++it) {
    switch (members[i].type) {
    case 'b':
      if (i && (members[i - 1].member.b == members[i].member.b)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.b == it->b);
      break ;
    case 'd':
      if (i && (members[i - 1].member.d == members[i].member.d)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.d == it->d);
      break ;
    case 'i':
      if (i && (members[i - 1].member.i == members[i].member.i)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.i == it->i);
      break ;
    case 's':
      if (i && (members[i - 1].member.s == members[i].member.s)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.s == it->s);
      break ;
    case 'S':
      if (i && (members[i - 1].member.S == members[i].member.S)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.S == it->S);
      break ;
    case 't':
      if (i && (members[i - 1].member.t == members[i].member.t)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.t == it->t);
      break ;
    case 'u':
      if (i && (members[i - 1].member.u == members[i].member.u)) {
        --it;
        continue ;
      }
      retval = (t.*members[i].member.u == it->u);
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
