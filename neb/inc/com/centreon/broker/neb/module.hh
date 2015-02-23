/*
** Copyright 2009-2012,2015 Merethis
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

#ifndef CCB_NEB_MODULE_HH
#  define CCB_NEB_MODULE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class module module.hh "com/centreon/broker/neb/module.hh"
   *  @brief Represents a module loaded in a Nagios instance.
   *
   *  The scheduling engine supports modules that extend its
   *  original features. This class describes such modules.
   */
  class            module : public io::data {
  public:
                   module();
                   module(module const& other);
                   ~module();
    module&        operator=(module const& other);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    QString        args;
    bool           enabled;
    QString        filename;
    bool           loaded;
    bool           should_be_loaded;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(module const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_MODULE_HH
