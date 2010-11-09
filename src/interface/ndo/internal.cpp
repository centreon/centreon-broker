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
static void static_init()
{
  for (unsigned int i = 0; MappedType<T>::members[i].type; ++i)
    if (MappedType<T>::members[i].id)
      {
        GetterSetter<T>& gs(NDOMappedType<T>::map[MappedType<T>::members[i].id]);

        gs.member = &MappedType<T>::members[i].member;
        switch (MappedType<T>::members[i].type)
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

template <> std::map<int, GetterSetter<Events::Acknowledgement> >
  Interface::NDO::NDOMappedType<Events::Acknowledgement>::map =
    std::map<int, GetterSetter<Events::Acknowledgement> >();
template <> std::map<int, GetterSetter<Events::comment> >
  Interface::NDO::NDOMappedType<Events::comment>::map =
    std::map<int, GetterSetter<Events::comment> >();
template <> std::map<int, GetterSetter<Events::custom_variable> >
  Interface::NDO::NDOMappedType<Events::custom_variable>::map =
    std::map<int, GetterSetter<Events::custom_variable> >();
template <> std::map<int, GetterSetter<Events::Downtime> >
  Interface::NDO::NDOMappedType<Events::Downtime>::map =
    std::map<int, GetterSetter<Events::Downtime> >();
template <> std::map<int, GetterSetter<Events::event_handler> >
  Interface::NDO::NDOMappedType<Events::event_handler>::map =
    std::map<int, GetterSetter<Events::event_handler> >();
template <> std::map<int, GetterSetter<Events::flapping_status> >
  Interface::NDO::NDOMappedType<Events::flapping_status>::map =
    std::map<int, GetterSetter<Events::flapping_status> >();
template <> std::map<int, GetterSetter<Events::Host> >
  Interface::NDO::NDOMappedType<Events::Host>::map =
    std::map<int, GetterSetter<Events::Host> >();
template <> std::map<int, GetterSetter<Events::HostCheck> >
  Interface::NDO::NDOMappedType<Events::HostCheck>::map =
    std::map<int, GetterSetter<Events::HostCheck> >();
template <> std::map<int, GetterSetter<Events::HostDependency> >
  Interface::NDO::NDOMappedType<Events::HostDependency>::map =
    std::map<int, GetterSetter<Events::HostDependency> >();
template <> std::map<int, GetterSetter<Events::HostGroup> >
  Interface::NDO::NDOMappedType<Events::HostGroup>::map =
    std::map<int, GetterSetter<Events::HostGroup> >();
template <> std::map<int, GetterSetter<Events::HostGroupMember> >
  Interface::NDO::NDOMappedType<Events::HostGroupMember>::map =
    std::map<int, GetterSetter<Events::HostGroupMember> >();
template <> std::map<int, GetterSetter<Events::HostParent> >
  Interface::NDO::NDOMappedType<Events::HostParent>::map =
    std::map<int, GetterSetter<Events::HostParent> >();
template <> std::map<int, GetterSetter<Events::HostStatus> >
  Interface::NDO::NDOMappedType<Events::HostStatus>::map =
    std::map<int, GetterSetter<Events::HostStatus> >();
template <> std::map<int, GetterSetter<Events::Issue> >
  Interface::NDO::NDOMappedType<Events::Issue>::map =
    std::map<int, GetterSetter<Events::Issue> >();
template <> std::map<int, GetterSetter<Events::IssueParent> >
  Interface::NDO::NDOMappedType<Events::IssueParent>::map =
    std::map<int, GetterSetter<Events::IssueParent> >();
template <> std::map<int, GetterSetter<Events::Log> >
  Interface::NDO::NDOMappedType<Events::Log>::map =
    std::map<int, GetterSetter<Events::Log> >();
template <> std::map<int, GetterSetter<Events::notification> >
  Interface::NDO::NDOMappedType<Events::notification>::map =
    std::map<int, GetterSetter<Events::notification> >();
template <> std::map<int, GetterSetter<Events::Program> >
  Interface::NDO::NDOMappedType<Events::Program>::map =
    std::map<int, GetterSetter<Events::Program> >();
template <> std::map<int, GetterSetter<Events::ProgramStatus> >
  Interface::NDO::NDOMappedType<Events::ProgramStatus>::map =
    std::map<int, GetterSetter<Events::ProgramStatus> >();
template <> std::map<int, GetterSetter<Events::Service> >
  Interface::NDO::NDOMappedType<Events::Service>::map =
    std::map<int, GetterSetter<Events::Service> >();
template <> std::map<int, GetterSetter<Events::ServiceCheck> >
  Interface::NDO::NDOMappedType<Events::ServiceCheck>::map =
    std::map<int, GetterSetter<Events::ServiceCheck> >();
template <> std::map<int, GetterSetter<Events::ServiceDependency> >
  Interface::NDO::NDOMappedType<Events::ServiceDependency>::map =
    std::map<int, GetterSetter<Events::ServiceDependency> >();
template <> std::map<int, GetterSetter<Events::ServiceGroup> >
  Interface::NDO::NDOMappedType<Events::ServiceGroup>::map =
    std::map<int, GetterSetter<Events::ServiceGroup> >();
template <> std::map<int, GetterSetter<Events::ServiceGroupMember> >
  Interface::NDO::NDOMappedType<Events::ServiceGroupMember>::map =
    std::map<int, GetterSetter<Events::ServiceGroupMember> >();
template <> std::map<int, GetterSetter<Events::ServiceStatus> >
  Interface::NDO::NDOMappedType<Events::ServiceStatus>::map =
    std::map<int, GetterSetter<Events::ServiceStatus> >();
template <> std::map<int, GetterSetter<Events::state> >
  Interface::NDO::NDOMappedType<Events::state>::map =
    std::map<int, GetterSetter<Events::state> >();

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
  static_init<Events::Acknowledgement>();
  static_init<Events::comment>();
  static_init<Events::custom_variable>();
  static_init<Events::Downtime>();
  static_init<Events::event_handler>();
  static_init<Events::flapping_status>();
  static_init<Events::Host>();
  static_init<Events::HostCheck>();
  static_init<Events::HostDependency>();
  static_init<Events::HostGroup>();
  static_init<Events::HostGroupMember>();
  static_init<Events::HostParent>();
  static_init<Events::HostStatus>();
  static_init<Events::Issue>();
  static_init<Events::IssueParent>();
  static_init<Events::Log>();
  static_init<Events::notification>();
  static_init<Events::Program>();
  static_init<Events::ProgramStatus>();
  static_init<Events::Service>();
  static_init<Events::ServiceCheck>();
  static_init<Events::ServiceDependency>();
  static_init<Events::ServiceGroup>();
  static_init<Events::ServiceGroupMember>();
  static_init<Events::ServiceStatus>();
  static_init<Events::state>();
  return ;
}
