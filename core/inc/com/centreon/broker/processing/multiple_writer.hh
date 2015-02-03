/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_PROCESSING_MULTIPLE_WRITER_HH
#  define CCB_PROCESSING_MULTIPLE_WRITER_HH

#  include <vector>
#  include <list>
#  include <string>
#  include <set>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {

      namespace          processing {
      // Foward declaration.
      class failover;

        /**
         *  @class multiple_writer multiple_writer.hh "com/centreon/broker/processing/multiple_writer.hh"
         *  @brief multiple_writer thread.
         *
         *  Class that synchronizes a write to multiple endpoints.
         */
        class      multiple_writer {
         public:
                   multiple_writer();
                   multiple_writer(multiple_writer const& right);
                   ~multiple_writer();
          multiple_writer&
                   operator=(multiple_writer const& right);

          void     set_primary_output(io::stream* output);
          void     register_secondary_endpoints(
                     std::string const& failover_name,
                     std::vector<misc::shared_ptr<io::endpoint> >&
                                                        secondary_endpoints);

          void     update();

          unsigned int
                   write(misc::shared_ptr<io::data> const& d);

         private:
          io::stream*
                   _primary_output;
          std::list<misc::shared_ptr<io::stream> >
                   _secondary_outputs;
          std::vector<misc::shared_ptr<io::endpoint> >*
                   _secondary_endpoints;
          std::string
                   _name;
        };
      }
    }
  }
}

#endif // !CCB_PROCESSING_MULTIPLE_WRITER_HH
