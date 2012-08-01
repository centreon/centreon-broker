/*
** Copyright 2011-2012 Merethis
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

#include <memory>
#include "com/centreon/broker/compression/factory.hh"
#include "com/centreon/broker/compression/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

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
factory::factory(factory const& f) : io::factory(f) {}

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
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return A copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint configuration match the compression layer.
 *
 *  @param[in] cfg       Configuration object.
 *  @param[in] is_input  Unused.
 *  @param[in] is_output Unused.
 *
 *  @return true if the configuration matches the compression layer.
 */
bool factory::has_endpoint(
                config::endpoint const& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("compression"));
  return ((cfg.params.end() != it)
          && (!it.value().compare("yes", Qt::CaseInsensitive)
              || !it.value().compare("enabled", Qt::CaseInsensitive)
              || it.value().toUInt()));
}

/**
 *  Create an endpoint matching the configuration object.
 *
 *  @param[in] cfg       Configuration object.
 *  @param[in] is_input  true if the endpoint should be an input object.
 *  @param[in] is_output true if the endpoint should be an output
 *                       object.
 *
 *  @return New endpoint object.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint const& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  (void)is_input;
  (void)is_output;
  (void)is_acceptor;

  // Get compression level.
  int level(-1);
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("compression_level"));
  if (it != cfg.params.end())
    level = it.value().toInt();

  // Get buffer size.
  unsigned int size(0);
  it = cfg.params.find("compression_buffer");
  if (it != cfg.params.end())
    size = it.value().toUInt();

  // Create compression object.
  std::auto_ptr<compression::opener> openr(new compression::opener);
  openr->set_level(level);
  openr->set_size(size);
  return (openr.release());
}
