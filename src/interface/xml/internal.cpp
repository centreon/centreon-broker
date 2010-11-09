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
static void static_init()
{
  for (unsigned int i = 0; MappedType<T>::members[i].type; ++i)
    if (MappedType<T>::members[i].name)
      {
        GetterSetter<T>&
          gs(XMLMappedType<T>::map[MappedType<T>::members[i].name]);

        gs.member = &MappedType<T>::members[i].member;
        // XXX : setters are not set.
        switch (MappedType<T>::members[i].type)
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

template <> std::map<std::string, GetterSetter<Events::Acknowledgement> >
  Interface::XML::XMLMappedType<Events::Acknowledgement>::map =
    std::map<std::string, GetterSetter<Events::Acknowledgement> >();
template <> std::map<std::string, GetterSetter<Events::comment> >
  Interface::XML::XMLMappedType<Events::comment>::map =
    std::map<std::string, GetterSetter<Events::comment> >();
template <> std::map<std::string, GetterSetter<Events::custom_variable> >
  Interface::XML::XMLMappedType<Events::custom_variable>::map =
    std::map<std::string, GetterSetter<Events::custom_variable> >();
template <> std::map<std::string, GetterSetter<Events::Downtime> >
  Interface::XML::XMLMappedType<Events::Downtime>::map =
    std::map<std::string, GetterSetter<Events::Downtime> >();
template <> std::map<std::string, GetterSetter<Events::event_handler> >
  Interface::XML::XMLMappedType<Events::event_handler>::map =
    std::map<std::string, GetterSetter<Events::event_handler> >();
template <> std::map<std::string, GetterSetter<Events::flapping_status> >
  Interface::XML::XMLMappedType<Events::flapping_status>::map =
    std::map<std::string, GetterSetter<Events::flapping_status> >();
template <> std::map<std::string, GetterSetter<Events::Host> >
  Interface::XML::XMLMappedType<Events::Host>::map =
    std::map<std::string, GetterSetter<Events::Host> >();
template <> std::map<std::string, GetterSetter<Events::HostCheck> >
  Interface::XML::XMLMappedType<Events::HostCheck>::map =
    std::map<std::string, GetterSetter<Events::HostCheck> >();
template <> std::map<std::string, GetterSetter<Events::HostDependency> >
  Interface::XML::XMLMappedType<Events::HostDependency>::map =
    std::map<std::string, GetterSetter<Events::HostDependency> >();
template <> std::map<std::string, GetterSetter<Events::HostGroup> >
  Interface::XML::XMLMappedType<Events::HostGroup>::map =
    std::map<std::string, GetterSetter<Events::HostGroup> >();
template <> std::map<std::string, GetterSetter<Events::HostGroupMember> >
  Interface::XML::XMLMappedType<Events::HostGroupMember>::map =
    std::map<std::string, GetterSetter<Events::HostGroupMember> >();
template <> std::map<std::string, GetterSetter<Events::HostParent> >
  Interface::XML::XMLMappedType<Events::HostParent>::map =
    std::map<std::string, GetterSetter<Events::HostParent> >();
template <> std::map<std::string, GetterSetter<Events::HostStatus> >
  Interface::XML::XMLMappedType<Events::HostStatus>::map =
    std::map<std::string, GetterSetter<Events::HostStatus> >();
template <> std::map<std::string, GetterSetter<Events::Issue> >
  Interface::XML::XMLMappedType<Events::Issue>::map =
    std::map<std::string, GetterSetter<Events::Issue> >();
template <> std::map<std::string, GetterSetter<Events::IssueParent> >
  Interface::XML::XMLMappedType<Events::IssueParent>::map =
    std::map<std::string, GetterSetter<Events::IssueParent> >();
template <> std::map<std::string, GetterSetter<Events::Log> >
  Interface::XML::XMLMappedType<Events::Log>::map =
    std::map<std::string, GetterSetter<Events::Log> >();
template <> std::map<std::string, GetterSetter<Events::notification> >
  Interface::XML::XMLMappedType<Events::notification>::map =
    std::map<std::string, GetterSetter<Events::notification> >();
template <> std::map<std::string, GetterSetter<Events::Program> >
  Interface::XML::XMLMappedType<Events::Program>::map =
    std::map<std::string, GetterSetter<Events::Program> >();
template <> std::map<std::string, GetterSetter<Events::ProgramStatus> >
  Interface::XML::XMLMappedType<Events::ProgramStatus>::map =
    std::map<std::string, GetterSetter<Events::ProgramStatus> >();
template <> std::map<std::string, GetterSetter<Events::Service> >
  Interface::XML::XMLMappedType<Events::Service>::map =
    std::map<std::string, GetterSetter<Events::Service> >();
template <> std::map<std::string, GetterSetter<Events::ServiceCheck> >
  Interface::XML::XMLMappedType<Events::ServiceCheck>::map =
    std::map<std::string, GetterSetter<Events::ServiceCheck> >();
template <> std::map<std::string, GetterSetter<Events::ServiceDependency> >
  Interface::XML::XMLMappedType<Events::ServiceDependency>::map =
    std::map<std::string, GetterSetter<Events::ServiceDependency> >();
template <> std::map<std::string, GetterSetter<Events::ServiceGroup> >
  Interface::XML::XMLMappedType<Events::ServiceGroup>::map =
    std::map<std::string, GetterSetter<Events::ServiceGroup> >();
template <> std::map<std::string, GetterSetter<Events::ServiceGroupMember> >
  Interface::XML::XMLMappedType<Events::ServiceGroupMember>::map =
    std::map<std::string, GetterSetter<Events::ServiceGroupMember> >();
template <> std::map<std::string, GetterSetter<Events::ServiceStatus> >
  Interface::XML::XMLMappedType<Events::ServiceStatus>::map =
    std::map<std::string, GetterSetter<Events::ServiceStatus> >();
template <> std::map<std::string, GetterSetter<Events::state> >
  Interface::XML::XMLMappedType<Events::state>::map =
    std::map<std::string, GetterSetter<Events::state> >();

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
