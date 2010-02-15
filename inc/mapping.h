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

#ifndef MAPPING_H_
# define MAPPING_H_

# include "events/events.h"

/**
 *  \class MappedData mapping.h "mapping.h"
 *  \brief Mapping of a data member.
 *
 *  This template represents the overall mapping of a specific data member of a
 *  class T. This data member is associated with a name (used by DB and XML)
 *  and an ID (used by NDO).
 */
template        <typename T>
class           MappedData
{
 public:
  enum          Type
  {
    UNKNOWN = '\0',
    BOOL = 'b',
    DOUBLE = 'd',
    INT = 'i',
    SHORT = 's',
    STRING = 'S',
    TIME_T = 't'
  };
  unsigned int  id;
  union
  {
    bool        T::*b;
    double      T::*d;
    int         T::*i;
    short       T::*s;
    std::string T::*S;
    time_t      T::*t;
  }             member;
  const char*   name;
  char          type;

                MappedData() : id(0), name(NULL), type(UNKNOWN) {}

                MappedData(bool T::* b, unsigned int i, const char* n)
    : id(i), name(n), type(BOOL)
  { this->member.b = b; }

                MappedData(double T::* d, unsigned int i, const char* n)
    : id(i), name(n), type(DOUBLE)
  { this->member.d = d; }

                MappedData(int T::* I, unsigned int i, const char* n)
    : id(i), name(n), type(INT)
  { this->member.i = I; }

                MappedData(short T::* s, unsigned int i, const char* n)
    : id(i), name(n), type(SHORT)
  { this->member.s = s; }

                MappedData(std::string T::* S, unsigned int i, const char* n)
    : id(i), name(n), type(STRING)
  { this->member.S = S; }

                MappedData(time_t T::* t, unsigned int i, const char* n)
    : id(i), name(n), type(TIME_T)
  { this->member.t = t; }
};

// Mappings.
extern const MappedData<Events::Acknowledgement>
  acknowledgement_mapping[];
extern const MappedData<Events::Comment>
  comment_mapping[];
extern const MappedData<Events::Downtime>
  downtime_mapping[];
extern const MappedData<Events::Host>
  host_mapping[];
extern const MappedData<Events::HostCheck>
  host_check_mapping[];
extern const MappedData<Events::HostDependency>
  host_dependency_mapping[];
extern const MappedData<Events::HostGroup>
  host_group_mapping[];
extern const MappedData<Events::HostGroupMember>
  host_group_member_mapping[];
extern const MappedData<Events::HostParent>
  host_parent_mapping[];
extern const MappedData<Events::HostStatus>
  host_status_mapping[];
extern const MappedData<Events::Log>
  log_mapping[];
extern const MappedData<Events::ProgramStatus>
  program_status_mapping[];
extern const MappedData<Events::Service>
  service_mapping[];
extern const MappedData<Events::ServiceCheck>
  service_check_mapping[];
extern const MappedData<Events::ServiceDependency>
  service_dependency_mapping[];
extern const MappedData<Events::ServiceGroup>
  service_group_mapping[];
extern const MappedData<Events::ServiceGroupMember>
  service_group_member_mapping[];
extern const MappedData<Events::ServiceStatus>
  service_status_mapping[];

#endif /* !MAPPING_H_ */
