/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_MAPPING_HH
#  define CCB_MAPPING_HH

#  include <QString>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

/**
 *  Holds a pointer to member.
 */
template         <typename T>
union            data_member {
  bool           T::*b;
  double         T::*d;
  int            T::*i;
  short          T::*s;
  QString        T::*S;
  timestamp      T::*t;
  unsigned int   T::*u;
};

/**
 *  @class mapped_data mapping.hh "mapping.hh"
 *  @brief Mapping of a data member.
 *
 *  This template represents the overall mapping of a specific
 *  data member of a class T. This data member is associated with
 *  a name (used by DB and XML) and an ID (used by NDO).
 */
template         <typename T>
class            mapped_data {
public:
  enum           Type {
    UNKNOWN = '\0',
    BOOL = 'b',
    DOUBLE = 'd',
    INT = 'i',
    SHORT = 's',
    STRING = 'S',
    TIMESTAMP = 't',
    UINT = 'u'
  };
  unsigned int   id;
  bool           is_id;
  data_member<T> member;
  char const*    name;
  bool           null_on_zero;
  char           type;

                 mapped_data() : id(0), name(NULL), type(UNKNOWN) {}

                 mapped_data(
                   bool T::* b,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(BOOL)
  { member.b = b; }

                 mapped_data(
                   double T::* d,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(DOUBLE)
  { member.d = d; }

                 mapped_data(
                   int T::* I,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(INT)
  { member.i = I; }

                 mapped_data(
                   short T::* s,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(SHORT)
  { member.s = s; }

                 mapped_data(
                   QString T::* S,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(STRING)
  { member.S = S; }

                 mapped_data(
                   timestamp T::* t,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(TIMESTAMP)
  { member.t = t; }

                 mapped_data(
                   unsigned int T::* u,
                   unsigned int i,
                   char const* n,
                   bool noz = false)
    : id(i), name(n), null_on_zero(noz), type(UINT)
  { member.u = u; }
};

template                       <typename T>
struct                         mapped_type {
  static mapped_data<T> const* members;
  static char const*           table;
};

CCB_END()

#endif // !CCB_MAPPING_HH
