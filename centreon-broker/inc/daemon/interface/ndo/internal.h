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
     *  This template is used to store pointer to members of all events.
     */
    template <typename T>
    struct   KeyField
    {
      // Holds a getter and a setter.
      struct GetterSetter
      {
	std::string (* getter)(const T&);
	void (* setter)(T&, const char*);
      };
      int    key;
      char   type;
      union  UHandler
      {
       public:
        bool (T::* field_bool);
        double (T::* field_double);
        int (T::* field_int);
        short (T::* field_short);
        std::string (T::* field_string);
        time_t (T::* field_timet);
	GetterSetter field_undefined;
      }      field;

      KeyField() : key(0), type('\0')
      { this->field.field_bool = NULL; }
      KeyField(int k, bool (T::* b)) : key(k), type('b')
      { this->field.field_bool = b; }
      KeyField(int k, double (T::* d)) : key(k), type('d')
      { this->field.field_double = d; }
      KeyField(int k, int (T::* i)) : key(k), type('i')
      { this->field.field_int = i; }
      KeyField(int k, short (T::* s)) : key(k), type('s')
      { this->field.field_short = s; }
      KeyField(int k, std::string (T::* s)) : key(k), type('S')
      { this->field.field_string = s; }
      KeyField(int k, time_t (T::* t)) : key(k), type('t')
      { this->field.field_timet = t; }
      KeyField(int k,
               std::string (* getter)(const T&),
               void (* setter)(T&, const char*)) : key(k), type('u')
      { this->field.field_undefined.getter = getter;
        this->field.field_undefined.setter = setter; }
    };

    // External arrays of pointer-to-members.
    extern const KeyField<Events::Acknowledgement> acknowledgement_fields[];
    extern const KeyField<Events::Comment>         comment_fields[];
    extern const KeyField<Events::Downtime>        downtime_fields[];
    extern const KeyField<Events::Host>            host_fields[];
    extern const KeyField<Events::HostGroup>       host_group_fields[];
    extern const KeyField<Events::HostStatus>      host_status_fields[];
    extern const KeyField<Events::ProgramStatus>   program_status_fields[];
    extern const KeyField<Events::Service>         service_fields[];
    extern const KeyField<Events::ServiceStatus>   service_status_fields[];
  }
}

#endif /* !INTERFACE_NDO_INTERNAL_H_ */
