/*
** Copyright 2013,2015 Merethis
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

#include <QDomDocument>
#include <QDomElement>
#include "com/centreon/broker/config/state.hh"
#include "test/bench_generate_rrd_mod.hh"

static benchmark* thread(NULL);

/**
 *  Get parameter.
 *
 *  @param[in] param The param on xml format.
 *
 *  @return The value.
 */
static unsigned int _get_param(std::string const& param) {
  // Parse XML.
  QDomDocument d;
  if (d.setContent(static_cast<QString>(param.c_str()))) {
    // Browse first-level elements.
    QDomElement elem(d.documentElement());
    if (!elem.isNull())
      return (elem.text().toUInt());
  }
  return (0);
}

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    delete thread;
    thread = NULL;
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration object.
   */
  void broker_module_init(config::state const* cfg) {
    std::map<std::string, std::string>::const_iterator it1, it2;
    it1 = cfg->params().find("bench_services");
    it2 = cfg->params().find("bench_requests_per_service");
    if (it1 != cfg->params().end() && it2 != cfg->params().end()) {
      unsigned int services(_get_param(it1->second));
      unsigned int requests_per_service(_get_param(it2->second));
      thread = new benchmark(services, requests_per_service);
      thread->start();
    }
    return ;
  }
}
