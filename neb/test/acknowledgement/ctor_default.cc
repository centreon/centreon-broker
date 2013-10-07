/*
** Copyright 2012-2013 Merethis
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

#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check acknowledgement's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::acknowledgement ack;

  // Check.
  return ((ack.acknowledgement_type != 0)
          || (ack.author != "")
          || (ack.comment != "")
          || (ack.deletion_time != 0)
          || (ack.entry_time != 0)
          || (ack.host_id != 0)
          || (ack.instance_id != 0)
          || (ack.is_sticky != false)
          || (ack.notify_contacts != false)
          || (ack.persistent_comment != false)
          || (ack.service_id != 0)
          || (ack.state != 0)
          || (ack.type()
              != io::data::data_type(io::data::neb, neb::de_acknowledgement)));
}
