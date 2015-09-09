/*
** Copyright 2011-2013 Centreon
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

#include <memory>
#include "com/centreon/broker/config/applier/temporary.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/temporary.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

// Class instance.
static temporary* gl_instance = NULL;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Apply a configuration.
 *
 *  @param[in] cfg  Temporary endpoint configuration.
 */
void temporary::apply(config::endpoint cfg) {
  if (!cfg.name.isEmpty() && !cfg.params["path"].isEmpty()) {
    misc::shared_ptr<io::endpoint> endp(_create_temporary(cfg));
    io::temporary::instance().set(endp);
  }
}

/**
 *  Get the instance of this object.
 *
 *  @return Class instance.
 */
temporary& temporary::instance() {
  return (*gl_instance);
}

/**
 *  Load singleton.
 */
void temporary::load() {
  if (!gl_instance)
    gl_instance = new temporary;
  return ;
}

/**
 *  Unload singleton.
 */
void temporary::unload() {
  delete gl_instance;
  gl_instance = NULL;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
temporary::temporary() {

}

/**
 *  Destructor.
 */
temporary::~temporary() {

}

/**
 *  Create new temporary.
 *
 *  @param[in] cfg The temporary configuration.
 *
 *  @return New temporary.
 */
misc::shared_ptr<io::endpoint> temporary::_create_temporary(
                                            config::endpoint& cfg) {
  // No temporary is define into the configuration.
  if (cfg.params.empty())
    return (NULL);

  // Debug message.
  logging::config(logging::medium)
    << "endpoint applier: creating new temporary '" << cfg.name << "'";

  // Check that failover is configured.
  if (!cfg.failover.isEmpty())
    throw (exceptions::msg() << "endpoint applier: find failover into "
           "temporary '" << cfg.name << "'");

  // Create endpoint object.
  misc::shared_ptr<io::endpoint> endp;
  bool is_acceptor(false);
  int level(0);
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it.value().osi_from == 1)
        && it.value().endpntfactry->has_endpoint(cfg, true, true)) {
      endp = misc::shared_ptr<io::endpoint>(
                     it.value().endpntfactry->new_endpoint(
                                                cfg,
                                                false,
                                                false,
                                                is_acceptor));
      level = it.value().osi_to + 1;
      break ;
    }
  }
  if (!endp)
    throw (exceptions::msg() << "endpoint applier: no matching " \
             "protocol found for temporary '" << cfg.name << "'");

  // Create remaining objects.
  while (level <= 7) {
    // Browse protocol list.
    QMap<QString, io::protocols::protocol>::const_iterator it(io::protocols::instance().begin());
    QMap<QString, io::protocols::protocol>::const_iterator end(io::protocols::instance().end());
    while (it != end) {
      if ((it.value().osi_from == level)
          && (it.value().endpntfactry->has_endpoint(cfg, true, true))) {
        misc::shared_ptr<io::endpoint>
          current(it.value().endpntfactry->new_endpoint(
                                             cfg,
                                             true,
                                             true,
                                             is_acceptor));
        current->from(endp);
        endp = current;
        level = it.value().osi_to;
        break ;
      }
      ++it;
    }
    if ((7 == level) && (it == end))
      throw (exceptions::msg() << "endpoint applier: no matching " \
               "protocol found for temporary '" << cfg.name << "'");
    ++level;
  }
  return (endp);
}
