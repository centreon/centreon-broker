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

#ifndef CCB_CORRELATION_NODE_HH
#  define CCB_CORRELATION_NODE_HH

#  include <memory>
#  include <map>
#  include <QList>
#  include <QHash>
#  include <QPair>
#  include "com/centreon/broker/correlation/issue.hh"
#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/neb/log_entry.hh"
#  include "com/centreon/broker/notification/downtime.hh"
#  include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace                correlation {
  /**
   *  @class node node.hh "com/centreon/broker/correlation/node.hh"
   *  @brief Node in the IT graph.
   *
   *  A node is an element of the IT infrastructure graph. It can
   *  either be an host or a service.
   */
  class                  node {
  public:
    typedef QHash<QPair<unsigned int, unsigned int>, node*>
                         node_map;

                         node();
                         node(node const& n);
                         ~node();
    node&                operator=(node const& n);
    bool                 operator==(node const& n) const;
    bool                 operator!=(node const& n) const;
    void                 add_child(node* n);
    void                 add_depended(node* n);
    void                 add_dependency(node* n);
    void                 add_parent(node* n);
    node_map const&      get_children() const;
    node_map const&      get_dependeds() const;
    node_map const&      get_dependencies() const;
    node_map const&      get_parents() const;
    void                 remove_child(node* n);
    void                 remove_depended(node* n);
    void                 remove_dependency(node* n);
    void                 remove_parent(node* n);
    QPair<unsigned int, unsigned int>
                         get_id() const;
    bool                 all_children_with_issues() const;

    void                 manage_status(
                          short status,
                          timestamp last_state_change,
                          io::stream* stream);
    void                 manage_ack(timestamp entry_time, io::stream* stream);
    void                 manage_downtime(
                           notification::downtime const& dwn,
                           io::stream* stream);
    void                 manage_downtime_removed(
                           unsigned int id,
                           io::stream* stream);
    void                 manage_log(
                           neb::log_entry const& entry,
                           io::stream* stream);
    enum                 link_type {
                         parent,
                         children,
                         depended_by,
                         depends_on
    };
    void                 linked_node_updated(
                           node& n,
                           timestamp start_time,
                           link_type type,
                           io::stream* stream);

    void                 serialize(persistent_cache& cache) const;

    unsigned int         host_id;
    unsigned int         instance_id;
    bool                 in_downtime;
    std::auto_ptr<issue> my_issue;
    std::auto_ptr<correlation::state>
                         my_state;
    unsigned int         service_id;
    short                state;
    std::map<unsigned int, notification::downtime>
                         downtimes;

   private:
    void                 _internal_copy(node const& n);

    node_map             _children;
    node_map             _depended_by;
    node_map             _depends_on;
    node_map             _parents;

    void                 _generate_state_event(
                           timestamp start_time,
                           short new_status,
                           io::stream* stream);
    correlation::state*  _open_state_event(
                           timestamp start_time) const;

  };
}

CCB_END()

#endif // !CCB_CORRELATION_NODE_HH
