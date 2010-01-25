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

#ifndef INTERFACE_FIELD_H_
# define INTERFACE_FIELD_H_

# include <list>
# include <string>
# include <time.h> // for time_t

namespace          Interface
{
  /**
   *  This template is used to store pointer to members of all events.
   */
  template         <typename T>
  struct           Field
  {
    // Holds a getter and a setter.
    struct         GetterSetter
    {
      std::string  (* getter)(const T&);
      void         (* setter)(T&, const char*);
    };

    union          FieldPointer
    {
     public:
      bool         (T::* field_bool);
      double       (T::* field_double);
      int          (T::* field_int);
      std::list<std::string>
                   (T::* field_list);
      short        (T::* field_short);
      std::string  (T::* field_string);
      time_t       (T::* field_timet);
      GetterSetter field_undefined;
      void         *field_void;
    }              field;
    char           type;

    // Constructors.
    Field() : type('\0')
    { this->field.field_bool = NULL; }
    Field(bool (T::* b)) : type('b')
    { this->field.field_bool = b; }
    Field(double (T::* d)) : type('d')
    { this->field.field_double = d; }
    Field(int (T::* i)) : type('i')
    { this->field.field_int = i; }
    Field(std::list<std::string> (T::* l)) : type('l')
    { this->field.field_list = l; }
    Field(short (T::* s)) : type('s')
    { this->field.field_short = s; }
    Field(std::string (T::* s)) : type('S')
    { this->field.field_string = s; }
    Field(time_t (T::* t)) : type('t')
    { this->field.field_timet = t; }
    // XXX : this is NDO-specific, should be replaced by void*
    Field(std::string (* getter)(const T&),
          void (* setter)(T&, const char*)) : type('u')
    { this->field.field_undefined.getter = getter;
      this->field.field_undefined.setter = setter; }
    // !XXX
    Field(void* ptr) : type('v')
    { this->field.field_void = ptr; }
  };
}

#endif /* !INTERFACE_FIELD_H_ */
