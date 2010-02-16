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
#include <stdlib.h>                 // for assert
#include "interface/xml/internal.h"

using namespace Interface::XML;

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
                        const std::string& name,
                        const DataMember<T>& member,
                        TiXmlElement& elem)
{
  elem.SetAttribute(name, (t.*(member.b) ? "true" : "false"));
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(const T& t,
                       const std::string& name,
                       const DataMember<T>& member,
                       TiXmlElement& elem)
{
  elem.SetAttribute(name, t.*(member.d));
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(const T& t,
                        const std::string& name,
                        const DataMember<T>& member,
                        TiXmlElement& elem)
{
  elem.SetAttribute(name, t.*(member.i));
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(const T& t,
                      const std::string& name,
                      const DataMember<T>& member,
                      TiXmlElement& elem)
{
  elem.SetAttribute(name, t.*(member.s));
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(const T& t,
                       const std::string& name,
                       const DataMember<T>& member,
                       TiXmlElement& elem)
{
  elem.SetAttribute(name, t.*(member.S));
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(const T& t,
                      const std::string& name,
                      const DataMember<T>& member,
                      TiXmlElement& elem)
{
  elem.SetAttribute(name, (int)(t.*(member.t)));
  return ;
}

/**
 *  Static initialization template used by Initialize().
 */
template <typename T>
static void static_init(const MappedData<T> members[],
                        std::map<std::string, GetterSetter<T> >& map)
{
  for (unsigned int i = 0; members[i].type; ++i)
    {
      GetterSetter<T>& gs(map[members[i].name]);

      gs.member = &members[i].member;
      // XXX : setters are not set.
      switch (members[i].type)
        {
         case MappedData<T>::BOOL:
          gs.getter = &get_boolean<T>;
          break ;
         case MappedData<T>::DOUBLE:
          gs.getter = &get_double<T>;
          break ;
         case MappedData<T>::INT:
          gs.getter = &get_integer<T>;
          break ;
         case MappedData<T>::SHORT:
          gs.getter = &get_short<T>;
          break ;
         case MappedData<T>::STRING:
          gs.getter = &get_string<T>;
          break ;
         case MappedData<T>::TIME_T:
          gs.getter = &get_timet<T>;
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

std::map<std::string, GetterSetter<Events::Acknowledgement> >
  Interface::XML::acknowledgement_map;
std::map<std::string, GetterSetter<Events::Comment> >
  Interface::XML::comment_map;
std::map<std::string, GetterSetter<Events::Downtime> >
  Interface::XML::downtime_map;
std::map<std::string, GetterSetter<Events::Host> >
  Interface::XML::host_map;
std::map<std::string, GetterSetter<Events::HostCheck> >
  Interface::XML::host_check_map;
std::map<std::string, GetterSetter<Events::HostDependency> >
  Interface::XML::host_dependency_map;
std::map<std::string, GetterSetter<Events::HostGroup> >
  Interface::XML::host_group_map;
std::map<std::string, GetterSetter<Events::HostGroupMember> >
  Interface::XML::host_group_member_map;
std::map<std::string, GetterSetter<Events::HostParent> >
  Interface::XML::host_parent_map;
std::map<std::string, GetterSetter<Events::HostStatus> >
  Interface::XML::host_status_map;
std::map<std::string, GetterSetter<Events::Log> >
  Interface::XML::log_map;
std::map<std::string, GetterSetter<Events::ProgramStatus> >
  Interface::XML::program_status_map;
std::map<std::string, GetterSetter<Events::Service> >
  Interface::XML::service_map;
std::map<std::string, GetterSetter<Events::ServiceCheck> >
  Interface::XML::service_check_map;
std::map<std::string, GetterSetter<Events::ServiceDependency> >
  Interface::XML::service_dependency_map;
std::map<std::string, GetterSetter<Events::ServiceGroup> >
  Interface::XML::service_group_map;
std::map<std::string, GetterSetter<Events::ServiceGroupMember> >
  Interface::XML::service_group_member_map;
std::map<std::string, GetterSetter<Events::ServiceStatus> >
  Interface::XML::service_status_map;

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  \brief Initialization routine.
 *
 *  Initialize XML mappings.
 */
void Interface::XML::Initialize()
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
