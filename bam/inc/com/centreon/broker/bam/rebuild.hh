/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_REBUILD_HH
#  define CCB_BAM_REBUILD_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class rebuild rebuild.hh "com/centreon/broker/bam/rebuild.hh"
   *  @brief ask for a rebuild.
   *
   *  This data event represent a rebuild asked.
   */
  class            rebuild : public io::data {
  public:
                   rebuild();
                   ~rebuild();
                   rebuild(rebuild const&);
    rebuild&       operator=(rebuild const&);
    bool           operator==(rebuild const& other) const;
    unsigned int   type() const;
    static unsigned int
                   static_type();

    QString        bas_to_rebuild;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(rebuild const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_REBUILD_HH
