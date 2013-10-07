/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_INSTANCE_HH
#  define CCB_NEB_INSTANCE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class instance instance.hh "com/centreon/broker/neb/instance.hh"
   *  @brief Information about Nagios process.
   *
   *  This class holds information about a Nagios process, like whether
   *  it is running or not, in daemon mode or not, ...
   */
  class            instance : public io::data {
  public:
                   instance();
                   instance(instance const& i);
                   ~instance();
    instance&      operator=(instance const& i);
    unsigned int   type() const;

    QString        engine;
    unsigned int   id;
    bool           is_running;
    QString        name;
    unsigned int   pid;
    timestamp      program_end;
    timestamp      program_start;
    QString        version;

  private:
    void           _internal_copy(instance const& i);
  };
}

CCB_END()

#endif // !CCB_NEB_INSTANCE_HH
