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

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACT_BY_NODE_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_CONTACT_BY_NODE_BUILDER_HH

#  include <vector>
#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/contact_by_name_builder.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/builders/contact_builder.hh"

CCB_BEGIN()

namespace       notification {

  class           contact_by_node_builder : public contact_builder {
  public:
                  contact_by_node_builder(QMultiHash<node_id, shared_ptr<contact> >& table);
    virtual       ~contact_by_node_builder() {}

    virtual void  add_contact(unsigned int id,
                             shared_ptr<contact> con);
    virtual void  connect_contact_node_id(unsigned int contact_id,
                                          node_id id);

  private:
    QHash<unsigned int, shared_ptr<contact> >
                  _cache;
    QMultiHash<node_id, shared_ptr<contact> >&
                  _table;
                  contact_by_node_builder();
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACT_BY_NODE_BUILDER_HH
