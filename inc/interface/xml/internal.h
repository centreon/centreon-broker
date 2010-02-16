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

#ifndef INTERFACE_XML_INTERNAL_H_
# define INTERFACE_XML_INTERNAL_H_

# include <map>
# include <string>
# include "events/events.h"
# include "mapping.h"
# include "interface/xml/tinyxml.h"

namespace                  Interface
{
  namespace                XML
  {
    template               <typename T>
    struct                 GetterSetter
    {
      const DataMember<T>* member;
      void                 (* getter)(const T&,
                                      const std::string& name,
                                      const DataMember<T>&,
                                      TiXmlElement& elem);
      void *               setter;
    };

    // XML mappings.
    extern std::map<std::string, GetterSetter<Events::Acknowledgement> >
      acknowledgement_map;
    extern std::map<std::string, GetterSetter<Events::Comment> >
      comment_map;
    extern std::map<std::string, GetterSetter<Events::Downtime> >
      downtime_map;
    extern std::map<std::string, GetterSetter<Events::Host> >
      host_map;
    extern std::map<std::string, GetterSetter<Events::HostCheck> >
      host_check_map;
    extern std::map<std::string, GetterSetter<Events::HostDependency> >
      host_dependency_map;
    extern std::map<std::string, GetterSetter<Events::HostGroup> >
      host_group_map;
    extern std::map<std::string, GetterSetter<Events::HostGroupMember> >
      host_group_member_map;
    extern std::map<std::string, GetterSetter<Events::HostParent> >
      host_parent_map;
    extern std::map<std::string, GetterSetter<Events::HostStatus> >
      host_status_map;
    extern std::map<std::string, GetterSetter<Events::Log> >
      log_map;
    extern std::map<std::string, GetterSetter<Events::ProgramStatus> >
      program_status_map;
    extern std::map<std::string, GetterSetter<Events::Service> >
      service_map;
    extern std::map<std::string, GetterSetter<Events::ServiceCheck> >
      service_check_map;
    extern std::map<std::string, GetterSetter<Events::ServiceDependency> >
      service_dependency_map;
    extern std::map<std::string, GetterSetter<Events::ServiceGroup> >
      service_group_map;
    extern std::map<std::string, GetterSetter<Events::ServiceGroupMember> >
      service_group_member_map;
    extern std::map<std::string, GetterSetter<Events::ServiceStatus> >
      service_status_map;

    // Mapping initialization routine.
    void Initialize();
  }
}

#endif /* !INTERFACE_XML_INTERNAL_H_ */
