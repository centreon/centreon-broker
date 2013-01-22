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

#ifndef CCB_MAPPING_SOURCE_HH_
# define CCB_MAPPING_SOURCE_HH_

# include <ctime>
# include <QString>
# include "com/centreon/broker/io/data.hh"

namespace                        com {
  namespace                      centreon {
    namespace                    broker {
      namespace                  mapping {
        /**
         *  @class source source.hh "com/centreon/broker/mapping/source.hh"
         *  @brief Internal mapping class.
         *
         *  This class is used internally by the mapping engine and
         *  should not be used otherwise.
         */
        class                    source {
         public:
          enum                   source_type {
            UNKNOWN = 0,
            BOOL,
            DOUBLE,
            INT,
            SHORT,
            STRING,
            TIME,
            UINT,
            USHORT
          };
                                 source();
                                 source(source const& s);
          virtual                ~source();
          source&                operator=(source const& s);
          virtual bool           get_bool(io::data const& d) = 0;
          virtual double         get_double(io::data const& d) = 0;
          virtual int            get_int(io::data const& d) = 0;
          virtual short          get_short(io::data const& d) = 0;
          virtual QString const& get_string(io::data const& d) = 0;
          virtual time_t         get_time(io::data const& d) = 0;
          virtual unsigned int   get_uint(io::data const& d) = 0;
          virtual unsigned short get_ushort(io::data const& d) = 0;
          virtual void           set_bool(io::data& d, bool value) = 0;
          virtual void           set_double(io::data& d,
                                   double value) = 0;
          virtual void           set_int(io::data& d, int value) = 0;
          virtual void           set_short(io::data& d,
                                   short value) = 0;
          virtual void           set_string(io::data& d,
                                   QString const& value) = 0;
          virtual void           set_time(io::data& d,
                                   time_t value) = 0;
          virtual void           set_uint(io::data& d,
                                   unsigned int value) = 0;
          virtual void           set_ushort(io::data& d,
                                   unsigned short value) = 0;
        };
      }
    }
  }
}

#endif /* !CCB_MAPPING_SOURCE_HH_ */
