/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_CORRELATION_NODE_HH_
# define CCB_CORRELATION_NODE_HH_

# include <QList>
# include <QScopedPointer>
# include "com/centreon/broker/correlation/issue.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 correlation {
  /**
   *  @class node node.hh "com/centreon/broker/correlation/node.hh"
   *  @brief Node in the IT graph.
   *
   *  A node is an element of the IT infrastructure graph. It can
   *  either be an host or a service.
   */
  class                   node {
   private:
    QList<node*>          _children;
    QList<node*>          _depended_by;
    QList<node*>          _depends_on;
    QList<node*>          _parents;
    void                  _internal_copy(node const& n);

   public:
    unsigned int          host_id;
    bool                  in_downtime;
    QScopedPointer<issue> my_issue;
    unsigned int          service_id;
    time_t                since;
    short                 state;
                          node();
                          node(node const& n);
                          ~node();
    node&                 operator=(node const& n);
    bool                  operator==(node const& n) const;
    bool                  operator!=(node const& n) const;
    void                  add_child(node* n);
    void                  add_depended(node* n);
    void                  add_dependency(node* n);
    void                  add_parent(node* n);
    QList<node*> const&   children() const throw ();
    QList<node*> const&   depended_by() const throw ();
    QList<node*> const&   depends_on() const throw ();
    QList<node*> const&   parents() const throw ();
    void                  remove_child(node* n);
    void                  remove_depended(node* n);
    void                  remove_dependency(node* n);
    void                  remove_parent(node* n);
  };
}

CCB_END()

#endif /* !CCB_CORRELATION_NODE_HH_ */
