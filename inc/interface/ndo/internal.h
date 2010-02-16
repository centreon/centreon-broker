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

#ifndef INTERFACE_NDO_INTERNAL_H_
# define INTERFACE_NDO_INTERNAL_H_

# include <map>
# include <sstream>
# include "events/events.h"
# include "mapping.h"

namespace                  Interface
{
  namespace                NDO
  {
    template               <typename T>
    struct                 GetterSetter
    {
      const DataMember<T>* member;
      void                 (* getter)(const T&,
                                      const DataMember<T>&,
                                      std::stringstream& buffer);
      void                 (* setter)(T&, const DataMember<T>&, const char*);
    };

    // NDO mappings.
    extern std::map<int, GetterSetter<Events::Acknowledgement> >
      acknowledgement_map;
    extern std::map<int, GetterSetter<Events::Comment> >
      comment_map;
    extern std::map<int, GetterSetter<Events::Downtime> >
      downtime_map;
    extern std::map<int, GetterSetter<Events::Host> >
      host_map;
    extern std::map<int, GetterSetter<Events::HostCheck> >
      host_check_map;
    extern std::map<int, GetterSetter<Events::HostDependency> >
      host_dependency_map;
    extern std::map<int, GetterSetter<Events::HostGroup> >
      host_group_map;
    extern std::map<int, GetterSetter<Events::HostGroupMember> >
      host_group_member_map;
    extern std::map<int, GetterSetter<Events::HostParent> >
      host_parent_map;
    extern std::map<int, GetterSetter<Events::HostStatus> >
      host_status_map;
    extern std::map<int, GetterSetter<Events::Log> >
      log_map;
    extern std::map<int, GetterSetter<Events::ProgramStatus> >
      program_status_map;
    extern std::map<int, GetterSetter<Events::Service> >
      service_map;
    extern std::map<int, GetterSetter<Events::ServiceCheck> >
      service_check_map;
    extern std::map<int, GetterSetter<Events::ServiceDependency> >
      service_dependency_map;
    extern std::map<int, GetterSetter<Events::ServiceGroup> >
      service_group_map;
    extern std::map<int, GetterSetter<Events::ServiceGroupMember> >
      service_group_member_map;
    extern std::map<int, GetterSetter<Events::ServiceStatus> >
      service_status_map;

    // Mapping initialization routine.
    void Initialize();
  }
}

#endif /* !INTERFACE_NDO_INTERNAL_H_ */
