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

#include <assert.h>
#include <sstream>
#include <stdlib.h>                 // for abort, strtol
#include "interface/ndo/internal.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Get a boolean from an object.
 */
template <typename T>
static void get_boolean(const T& t,
                        const DataMember<T>& member,
                        std::stringstream& buffer)
{
  buffer << (t.*(member.b) ? "1" : "0");
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(const T& t,
                       const DataMember<T>& member,
                       std::stringstream& buffer)
{
  buffer << t.*(member.d);
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(const T& t,
                        const DataMember<T>& member,
                        std::stringstream& buffer)
{
  buffer << t.*(member.i);
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(const T& t,
                      const DataMember<T>& member,
                      std::stringstream& buffer)
{
  buffer << t.*(member.s);
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(const T& t,
                       const DataMember<T>& member,
                       std::stringstream& buffer)
{
  buffer << t.*(member.S);
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(const T& t,
                      const DataMember<T>& member,
                      std::stringstream& buffer)
{
  buffer << t.*(member.t);
  return ;
}

/**
 *  Set a boolean within an object.
 */
template <typename T>
static void set_boolean(T& t,
                        const DataMember<T>& member,
                        const char* str)
{
  t.*(member.b) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a double within an object.
 */
template <typename T>
static void set_double(T& t,
                       const DataMember<T>& member,
                       const char* str)
{
  t.*(member.d) = strtod(str, NULL);
  return ;
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static void set_integer(T& t,
                        const DataMember<T>& member,
                        const char* str)
{
  t.*(member.i) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a short within an object.
 */
template <typename T>
static void set_short(T& t,
                      const DataMember<T>& member,
                      const char* str)
{
  t.*(member.s) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a string within an object.
 */
template <typename T>
static void set_string(T& t,
                       const DataMember<T>& member,
                       const char* str)
{
  t.*(member.S) = str;
  return ;
}

/**
 *  Set a time_t within an object.
 */
template <typename T>
static void set_timet(T& t,
                      const DataMember<T>& member,
                      const char* str)
{
  t.*(member.t) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Static initialization template used by Initialize().
 */
template <typename T>
static void static_init(const MappedData<T> members[],
                        std::map<int, GetterSetter<T> >& map)
{
  for (unsigned int i = 0; members[i].type; ++i)
    {
      GetterSetter<T>& gs(map[members[i].id]);

      gs.member = &members[i].member;
      switch (members[i].type)
        {
         case MappedData<T>::BOOL:
          gs.getter = &get_boolean<T>;
          gs.setter = &set_boolean<T>;
          break ;
         case MappedData<T>::DOUBLE:
          gs.getter = &get_double<T>;
          gs.setter = &set_double<T>;
          break ;
         case MappedData<T>::INT:
          gs.getter = &get_integer<T>;
          gs.setter = &set_integer<T>;
          break ;
         case MappedData<T>::SHORT:
          gs.getter = &get_short<T>;
          gs.setter = &set_short<T>;
          break ;
         case MappedData<T>::STRING:
          gs.getter = &get_string<T>;
          gs.setter = &set_string<T>;
          break ;
         case MappedData<T>::TIME_T:
          gs.getter = &get_timet<T>;
          gs.setter = &set_timet<T>;
          break ;
         default: // Error in one of the mappings.
          assert(false);
          abort();
        }
    }
  return ;
}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

std::map<int, GetterSetter<Events::Acknowledgement> >
  Interface::NDO::acknowledgement_map;
std::map<int, GetterSetter<Events::Comment> >
  Interface::NDO::comment_map;
std::map<int, GetterSetter<Events::Downtime> >
  Interface::NDO::downtime_map;
std::map<int, GetterSetter<Events::Host> >
  Interface::NDO::host_map;
std::map<int, GetterSetter<Events::HostCheck> >
  Interface::NDO::host_check_map;
std::map<int, GetterSetter<Events::HostDependency> >
  Interface::NDO::host_dependency_map;
std::map<int, GetterSetter<Events::HostGroup> >
  Interface::NDO::host_group_map;
std::map<int, GetterSetter<Events::HostGroupMember> >
  Interface::NDO::host_group_member_map;
std::map<int, GetterSetter<Events::HostParent> >
  Interface::NDO::host_parent_map;
std::map<int, GetterSetter<Events::HostStatus> >
  Interface::NDO::host_status_map;
std::map<int, GetterSetter<Events::Log> >
  Interface::NDO::log_map;
std::map<int, GetterSetter<Events::ProgramStatus> >
  Interface::NDO::program_status_map;
std::map<int, GetterSetter<Events::Service> >
  Interface::NDO::service_map;
std::map<int, GetterSetter<Events::ServiceCheck> >
  Interface::NDO::service_check_map;
std::map<int, GetterSetter<Events::ServiceDependency> >
  Interface::NDO::service_dependency_map;
std::map<int, GetterSetter<Events::ServiceGroup> >
  Interface::NDO::service_group_map;
std::map<int, GetterSetter<Events::ServiceGroupMember> >
  Interface::NDO::service_group_member_map;
std::map<int, GetterSetter<Events::ServiceStatus> >
  Interface::NDO::service_status_map;

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  \brief Initialization routine.
 *
 *  Initialize NDO mappings.
 */
void Interface::NDO::Initialize()
{
  static_init(acknowledgement_mapping, acknowledgement_map);
  static_init(comment_mapping, comment_map);
  static_init(downtime_mapping, downtime_map);
  static_init(host_mapping, host_map);
  static_init(host_check_mapping, host_check_map);
  static_init(host_dependency_mapping, host_dependency_map);
  static_init(host_group_mapping, host_group_map);
  static_init(host_group_member_mapping, host_group_member_map);
  static_init(host_parent_mapping, host_parent_map);
  static_init(host_status_mapping, host_status_map);
  static_init(log_mapping, log_map);
  static_init(program_status_mapping, program_status_map);
  static_init(service_mapping, service_map);
  static_init(service_check_mapping, service_check_map);
  static_init(service_dependency_mapping, service_dependency_map);
  static_init(service_group_mapping, service_group_map);
  static_init(service_group_member_mapping, service_group_member_map);
  static_init(service_status_mapping, service_status_map);
  return ;
}
