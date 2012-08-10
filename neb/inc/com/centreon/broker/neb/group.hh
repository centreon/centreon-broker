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

#ifndef CCB_NEB_GROUP_HH
#  define CCB_NEB_GROUP_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class group group.hh "com/centreon/broker/neb/group.hh"
   *  @brief Base of host and service group classes.
   *
   *  The scheduling engine handles group. This can be service groups
   *  or host groups for example.
   *
   *  @see host_group
   *  @see service_group
   */
  class          group : public io::data {
  public:
                 group();
                 group(group const& g);
    virtual      ~group();
    group&       operator=(group const& g);

    QString      action_url;
    QString      alias;
    bool         enabled;
    unsigned int instance_id;
    QString      name;
    QString      notes;
    QString      notes_url;

  private:
    void         _internal_copy(group const& g);
  };
}

CCB_END()

#endif // !CCB_NEB_GROUP_HH
