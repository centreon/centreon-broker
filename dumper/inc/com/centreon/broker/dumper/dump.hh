/*
** Copyright 2013 Merethis
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

#ifndef CCB_DUMPER_DUMP_HH
#  define CCB_DUMPER_DUMP_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        dumper {
  /**
   *  @class dump dump.hh "com/centreon/broker/dumper/dump.hh"
   *  @brief Dump of a file.
   *
   *  Dump of a file transmitted by some provider.
   */
  class          dump : public io::data {
  public:
                 dump();
                 dump(dump const& right);
                 ~dump();
    dump&        operator=(dump const& right);
    unsigned int type() const;

    std::string  content;
    unsigned int instance_id;
    std::string  tag;

  private:
    void         _internal_copy(dump const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DUMP_HH
