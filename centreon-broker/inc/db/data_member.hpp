/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef DB_DATA_MEMBER_HPP_
# define DB_DATA_MEMBER_HPP_

# include <string>

namespace           CentreonBroker
{
  namespace         DB
  {
    template        <typename T>
    struct          DataMember
    {
     public:
      const char*   name;
      union
      {
	bool        T::*b;
	double      T::*d;
	int         T::*i;
	short       T::*s;
	std::string T::*S;
	time_t      T::*t;
      }             value;
      char          type;

      DataMember() : name(NULL), type('\0') {}

      DataMember(const char* n, bool T::*b) : name(n), type('b')
      {	this->value.b = b; }

      DataMember(const char* n, double T::*d) : name(n), type('d')
      { this->value.d = d; }

      DataMember(const char* n, int T::*i) : name(n), type('i')
      { this->value.i = i; }

      DataMember(const char* n, short T::*s) : name(n), type('s')
      { this->value.s = s; }

      DataMember(const char* n, std::string T::*S) : name(n), type('S')
      { this->value.S = S; }

      DataMember(const char* n, time_t T::*t) : name(n), type('t')
      { this->value.t = t; }
    };
  }
}

#endif /* !DB_DATA_MEMBER_HPP_ */
