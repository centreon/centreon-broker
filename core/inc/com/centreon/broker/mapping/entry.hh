/*
** Copyright 2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <QSharedPointer>
#include "com/centreon/broker/mapping/property.hh"
#include "com/centreon/broker/mapping/source.hh"

namespace                        com {
  namespace                      centreon {
    namespace                    broker {
      namespace                  mapping {
        /**
         *  @class entry entry.hh "com/centreon/broker/mapping/entry.hh"
         *  @brief Mapping class.
         *
         *  Holds a member with a name and a data source.
         */
        class                    entry {
         private:
          QString                _name;
          source*                _ptr;
          QSharedPointer<source> _source;
          source::source_type    _type;

         public:
          /**
           *  @brief Constructor.
           *
           *  Build an entry from a property.
           *
           *  @param[in] name Entry name.
           *  @param[in] prop Property.
           */
          template <typename T, typename U>
                                 entry(U (T::* prop),
                                       QString const& name) {
            _name = name;
            _source = QSharedPointer<source>(new property<T>(prop, &_type));
            _ptr = _source.data();
          }

                                 entry();
                                 entry(entry const& e);
                                 ~entry();
          entry&                 operator=(entry const& e);
          bool                   get_bool(io::data const& d);
          double                 get_double(io::data const& d);
          int                    get_int(io::data const& d);
          short                  get_short(io::data const& d);
          QString const&         get_string(io::data const& d);
          time_t                 get_time(io::data const& d);
          unsigned int           get_uint(io::data const& d);
          unsigned short         get_ushort(io::data const& d);
          void                   set_bool(io::data& d, bool value);
          void                   set_double(io::data& d, double value);
          void                   set_int(io::data& d, int value);
          void                   set_short(io::data& d, short value);
          void                   set_string(io::data& d,
                                   QString const& value);
          void                   set_time(io::data& d, time_t value);
          void                   set_uint(io::data& d,
                                   unsigned int value);
          void                   set_ushort(io::data& d,
                                   unsigned short value);
        };
      }
    }
  }
}
