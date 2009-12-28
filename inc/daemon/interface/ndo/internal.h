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

# include <string>
# include "interface/field.h"

// Forward declarations.
namespace    Events
{
  class      Acknowledgement;
  class      Comment;
  class      Downtime;
  class      Host;
  class      HostGroup;
  class      HostStatus;
  class      ProgramStatus;
  class      Service;
  class      ServiceStatus;
}

namespace    Interface
{
  namespace  NDO
  {
    /**
     *  This template is used to associate a pointer to member with a NDO key.
     */
    template <typename T>
    struct   KeyField : public Interface::Field<T>
    {
      int    key;

      KeyField() : key(0) {}
      KeyField(int k, bool (T::* b)) : Interface::Field<T>(b), key(k) {}
      KeyField(int k, double (T::* d)) : Interface::Field<T>(d), key(k) {}
      KeyField(int k, int (T::* i)) : Interface::Field<T>(i), key(k) {}
      KeyField(int k, short (T::* s)) : Interface::Field<T>(s), key(k) {}
      KeyField(int k, std::string (T::* s)) : Interface::Field<T>(s), key(k) {}
      KeyField(int k, time_t (T::* t)) : Interface::Field<T>(t), key(k) {}
      KeyField(int k,
               std::string (* getter)(const T&),
               void (* setter)(T&, const char*))
        : Interface::Field<T>(getter, setter), key(k) {}
    };

    // External arrays of pointer-to-members.
    extern const KeyField<Events::Acknowledgement> acknowledgement_fields[];
    extern const KeyField<Events::Comment>         comment_fields[];
    extern const KeyField<Events::Downtime>        downtime_fields[];
    extern const KeyField<Events::Host>            host_fields[];
    extern const KeyField<Events::HostGroup>       host_group_fields[];
    extern const KeyField<Events::HostStatus>      host_status_fields[];
    extern const KeyField<Events::Log>             log_fields[];
    extern const KeyField<Events::ProgramStatus>   program_status_fields[];
    extern const KeyField<Events::Service>         service_fields[];
    extern const KeyField<Events::ServiceStatus>   service_status_fields[];
  }
}

#endif /* !INTERFACE_NDO_INTERNAL_H_ */
