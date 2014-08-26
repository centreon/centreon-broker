/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH

#  include <QHash>
#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/dependency.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace       notification {

  class           dependency_by_node_id_builder {
  public:
                  dependency_by_node_id_builder(
                        QMultiHash<node_id, shared_ptr<dependency> >& by_child_id,
                        QMultiHash<node_id, shared_ptr<dependency> >& by_parent_id);
    virtual       ~dependency_by_node_id_builder() {}

    virtual void  add_dependency(unsigned int id,
                                shared_ptr<dependency> d);
    virtual void  dependency_node_id_parent_relation(unsigned int dep_id,
                                                    node_id id);
    virtual void  dependency_node_id_child_relation(unsigned int dep_id,
                                                   node_id id);
  private:
                  dependency_by_node_id_builder();

     QHash<unsigned int, shared_ptr<dependency> >
                  _cache;
     QMultiHash<node_id, shared_ptr<dependency> >&
                  _table_child_id;
     QMultiHash<node_id, shared_ptr<dependency> >&
                  _table_parent_id;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH
