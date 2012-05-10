/*
** Copyright 2011 Merethis
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
#include <QSharedPointer>
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

// Correlation object.
static QSharedPointer<multiplexing::hooker> obj;

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances) {
      // Unregister correlation object.
      multiplexing::engine::instance().unhook(*obj);
      obj.clear();
    }
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration argument.
   */
  void broker_module_init(void const* arg) {
    // Increment instance number.
    if (!instances++) {
      // Check that correlation is enabled.
      config::state const& cfg(*static_cast<config::state const*>(arg));
      bool loaded(false);
      QMap<QString, QString>::const_iterator
        it(cfg.params().find("correlation"));
      if (it != cfg.params().end()) {
        // Parameters.
        QString correlation_file;
        QString retention_file;

        // Parse XML.
        QDomDocument d;
        if (d.setContent(it.value())) {
          // Browse first-level elements.
          QDomElement root(d.documentElement());
          QDomNodeList level1(root.childNodes());
          for (int i = 0, len = level1.size(); i < len; ++i) {
            QDomElement elem(level1.item(i).toElement());
            if (!elem.isNull()) {
              QString name(elem.tagName());
              if (name == "file")
                correlation_file = elem.text();
              else if (name == "retention")
                retention_file = elem.text();
            }
          }
        }

        // File exists, load it.
        if (!correlation_file.isEmpty()) {
          // Create and register correlation object.
          QSharedPointer<correlation::correlator>
            crltr(new correlation::correlator);
          try {
            crltr->load(correlation_file, retention_file);
            obj = crltr.staticCast<multiplexing::hooker>();
            multiplexing::engine::instance().hook(*obj);
            loaded = true;
          }
          catch (std::exception const& e) {
            logging::config(logging::high) << "correlation: " \
              "configuration loading error: " << e.what();
          }
          catch (...) {
            logging::config(logging::high) << "correlation: " \
              "configuration loading error";
          }
        }
      }
      if (!loaded)
        logging::config(logging::high) << "correlation: invalid " \
          "correlation configuration, correlation engine is NOT loaded";
    }
    return ;
  }
}
