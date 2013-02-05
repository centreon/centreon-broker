/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) {
  (void)f;
}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  (void)f;
  return (*this);
}

/**
 *  @brief Create a new stream.
 *
 *  This function is used to generate new streams after successful
 *  stream construction like for a feature negociation.
 *
 *  @param[in] to          Stream on which the stream will work.
 *  @param[in] is_acceptor true if stream must be an accepting stream.
 *  @param[in] proto_name  Protocol name.
 *
 *  @return New stream.
 */
misc::shared_ptr<stream> factory::new_stream(
                                    misc::shared_ptr<stream> to,
                                    bool is_acceptor,
                                    QString const& proto_name) {
  (void)to;
  (void)is_acceptor;
  throw (exceptions::msg() << proto_name
         << ": protocol does not support feature negociation");
  return (misc::shared_ptr<stream>());
}
