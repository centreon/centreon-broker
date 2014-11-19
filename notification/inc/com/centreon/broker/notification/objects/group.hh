/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NOTIFICATION_OBJECTS_GROUP_HH
#  define CCB_NOTIFICATION_OBJECTS_GROUP_HH

#  include <list>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

typedef std::list<std::string> list_string;

CCB_BEGIN()

namespace              notification {
  namespace            objects {
    class                group {
    public:
                         DECLARE_SHARED_PTR(group);

                         group(bool inherit = false);
                         group(group const& right);
                         ~group() throw ();
      group&             operator=(group const& right);
      group&             operator=(std::string const& right);
      group&             operator+=(group const& right);
      bool               operator==(group const& right) const throw ();
      bool               operator!=(group const& right) const throw ();
      bool               operator<(group const& right) const throw ();
      list_string&       operator*() throw () { return (_data); }
      list_string const& operator*() const throw () { return (_data); }
      list_string*       operator->() throw () { return (&_data); }
      list_string const* operator->() const throw () { return (&_data); }
      list_string&       get() throw () { return (_data); }
      list_string const& get() const throw () { return (_data); }
      bool               is_inherit() const throw () { return (_is_inherit); }
      void               is_inherit(bool enable) throw () { _is_inherit = enable; }
      bool               is_set() const throw () { return (_is_set); }
      void               reset();

    private:
      list_string        _data;
      bool               _is_inherit;
      bool               _is_null;
      bool               _is_set;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_OBJECTS_GROUP_HH

