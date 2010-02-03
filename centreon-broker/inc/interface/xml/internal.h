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

# include <stddef.h>          // for NULL
# include "interface/field.h"

// Forward declarations.
namespace         Events
{
  class           Acknowledgement;
  class           Comment;
  class           Downtime;
  class           Host;
  class           HostDependency;
  class           HostGroup;
  class           HostGroupMember;
  class           HostParent;
  class           HostStatus;
  class           ProgramStatus;
  class           Service;
  class           ServiceDependency;
  class           ServiceGroup;
  class           ServiceGroupMember;
  class           ServiceStatus;
}

namespace         Interface
{
  namespace       XML
  {
    /**
     *  This template associates a field name with a pointer to member.
     */
    template      <typename T>
    struct        NameField : public Interface::Field<T>
    {
      const char* name;

      NameField() : name(NULL) {}
      NameField(const char* n, bool (T::* b))
        : Interface::Field<T>(b), name(n) {}
      NameField(const char* n, double (T::* d))
        : Interface::Field<T>(d), name(n) {}
      NameField(const char* n, int (T::* i))
        : Interface::Field<T>(i), name(n) {}
      NameField(const char* n, short (T::* s))
        : Interface::Field<T>(s), name(n) {}
      NameField(const char* n, std::string (T::* s))
        : Interface::Field<T>(s), name(n) {}
      NameField(const char* n, time_t (T::* t))
        : Interface::Field<T>(t), name(n) {}
    };

    // External arrays of pointer-to-members.
    extern const NameField<Events::Acknowledgement> acknowledgement_fields[];
    extern const NameField<Events::Comment>         comment_fields[];
    extern const NameField<Events::Downtime>        downtime_fields[];
    extern const NameField<Events::Host>            host_fields[];
    extern const NameField<Events::HostDependency>  host_dependency_fields[];
    extern const NameField<Events::HostGroup>       host_group_fields[];
    extern const NameField<Events::HostGroupMember> host_group_member_fields[];
    extern const NameField<Events::HostParent>      host_parent_fields[];
    extern const NameField<Events::HostStatus>      host_status_fields[];
    extern const NameField<Events::Log>             log_fields[];
    extern const NameField<Events::ProgramStatus>   program_status_fields[];
    extern const NameField<Events::Service>         service_fields[];
    extern const NameField<Events::ServiceDependency>
      service_dependency_fields[];
    extern const NameField<Events::ServiceGroup>    service_group_fields[];
    extern const NameField<Events::ServiceGroupMember>
      service_group_member_fields[];
    extern const NameField<Events::ServiceStatus>   service_status_fields[];
  }
}

#endif /* !INTERFACE_XML_INTERNAL_H_ */
