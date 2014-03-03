/*
** Copyright 2009-2014 Merethis
**
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

#ifndef  CCB_CONFIGURATION_READER_HH
#  define CCB_CONFIGURATION_READER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam{
  namespace configuration{
    /**
     * class declarations
     *
     */
    class db;
    class state;
  
    /**
     *@class reader reader.hh "com/centreon/broker/bam/configuration/reader.hh"
     *
     *@brief using the dbinfo to access the database, load state_obj with configuration
     */
    class reader{
     
    public:
      reader();
      reader(const reader&);
      reader& operator=(const reader&);
      ~reader();

      void read(db const& dbinfo , state& state_obj);
    };
  }
}

CCB_END()

#endif //!CCB_CONFIGURATION_READER_HH
