/*
** Copyright 2009-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_CORRELATION_NODE_HH
#  define CCB_CORRELATION_NODE_HH

#  include <memory>
#  include <map>
#  include <set>
#  include "com/centreon/broker/correlation/issue.hh"
#  include "com/centreon/broker/correlation/state.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/log_entry.hh"
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
  class                  node : public correlation::state {
  public:
    typedef std::set<node*>
                         node_map;

                         node();
                         node(node const& other);
                         ~node();
    node&                operator=(node const& other);
    bool                 operator==(node const& other) const;
    bool                 operator!=(node const& other) const;
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
    std::pair<unsigned int, unsigned int>
                         get_id() const;
    bool                 all_parents_with_issues_and_get_start_time(
                           timestamp& start_time) const;

    void                 manage_status(
                          short status,
                          timestamp last_state_change,
                          io::stream* stream);
    void                 manage_ack(
                          neb::acknowledgement const& ack,
                          io::stream* stream);
    void                 manage_downtime(
                           neb::downtime const& dwn,
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
                           bool closed,
                           link_type type,
                           io::stream* stream);

    void                 serialize(persistent_cache& cache) const;

    std::unique_ptr<issue> my_issue;
    std::unique_ptr<neb::acknowledgement>
                         acknowledgement;
    std::map<unsigned int, neb::downtime>
                         downtimes;

   private:
    void                 _internal_copy(node const& other);
    void                 _generate_state_event(
                           timestamp start_time,
                           short new_status,
                           bool new_in_downtime,
                           io::stream* stream);
    correlation::state   _open_state_event(
                           timestamp start_time) const;

    void                 _visit_linked_nodes(
                           timestamp last_state_change,
                           bool closed,
                           io::stream* stream);
    void                 _visit_parent_of_child_nodes(
                           timestamp last_state_change,
                           bool closed,
                           io::stream* stream);

    node_map             _children;
    node_map             _depended_by;
    node_map             _depends_on;
    node_map             _parents;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_NODE_HH
