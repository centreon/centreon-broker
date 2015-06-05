/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_NEB_DOWNTIME_MAP_HH
#  define CCB_NEB_DOWNTIME_MAP_HH

#  include <QHash>
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/node_id.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class downtime_map downtime_map.hh "com/centreon/broker/neb/downtime_map.hh"
   *  @brief Map of downtimes.
   */
  class          downtime_map {
  public:
                 downtime_map();
                 downtime_map(
                   downtime_map const& other);
    downtime_map&
                 operator=(downtime_map const& other);
    virtual      ~downtime_map();

    unsigned int get_new_downtime_id();
    QList<downtime>
                 get_all_downtimes_of_node(node_id id) const;
    QList<downtime>
                 get_all_recurring_downtimes_of_node(node_id id) const;
    void         delete_downtime(downtime const& dwn);
    void         add_downtime(downtime const& dwn);
    downtime*    get_downtime(unsigned int internal_id);
    bool         is_recurring(unsigned int internal_id) const;
    QList<downtime>
                 get_all_recurring_downtimes() const;
    QList<downtime>
                 get_all_downtimes() const;
    bool         spawned_downtime_exist(unsigned int parent_id) const;

  private:
    unsigned int _actual_downtime_id;
    QHash<unsigned int, downtime>
                 _downtimes;
    QMultiHash<node_id, unsigned int>
                 _downtime_id_by_nodes;
    QHash<unsigned int, downtime>
                 _recurring_downtimes;
    QMultiHash<node_id, unsigned int>
                 _recurring_downtime_id_by_nodes;

  };
}

CCB_END()

#endif // !CCB_NEB_DOWNTIME_MAP_HH
