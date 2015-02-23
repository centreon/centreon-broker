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

#include <QDomDocument>
#include <QDomElement>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

CCB_BEGIN()
namespace correlation {
  // Load count.
  static unsigned int instances(0);

  // Correlation object.
  static misc::shared_ptr<multiplexing::hooker> obj;

  /**
   *  Module deinitialization routine.
   */
  void module_deinit() {
    // Decrement instance number.
    if (!--correlation::instances) {
      // Unregister correlation object.
      multiplexing::engine::instance().unhook(*correlation::obj);
      correlation::obj.clear();

      // Remove events.
      io::events::instance().unregister_category(io::events::correlation);
    }
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration argument.
   */
  void module_init(void const* arg) {
    // Increment instance number.
    if (!correlation::instances++) {
      // Correlation module.
      logging::info(logging::high)
        << "correlation: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      io::events& e(io::events::instance());

      // Register category.
      int correlation_category(e.register_category("correlation", io::events::correlation));
      if (correlation_category != io::events::correlation) {
        e.unregister_category(correlation_category);
        --correlation::instances;
        throw (exceptions::msg() << "correlation: category " << io::events::correlation
               << " is already registered whereas it should be "
               << "reserved for the correlation module");
      }

      // Register events.
      {
        e.register_event(
            io::events::correlation,
            correlation::de_engine_state,
            io::event_info(
                  "engine_state",
                  &correlation::engine_state::operations,
                  correlation::engine_state::entries));
        e.register_event(
            io::events::correlation,
            correlation::de_host_state,
            io::event_info(
                  "host_state",
                  &correlation::host_state::operations,
                  correlation::host_state::entries));
        e.register_event(
            io::events::correlation,
            correlation::de_issue,
            io::event_info(
                  "issue",
                  &correlation::issue::operations,
                  correlation::issue::entries));
        e.register_event(
            io::events::correlation,
            correlation::de_issue_parent,
            io::event_info(
                  "issue_parent",
                  &correlation::issue_parent::operations,
                  correlation::issue_parent::entries));
        e.register_event(
            io::events::correlation,
            correlation::de_service_state,
            io::event_info(
                  "service_state",
                  &correlation::service_state::operations,
                  correlation::service_state::entries));
      }

      // Check that correlation is enabled.
      config::state const& cfg(*static_cast<config::state const*>(arg));
      bool loaded(false);
      QMap<QString, QString>::const_iterator
        it(cfg.params().find("correlation"));
      if (it != cfg.params().end()) {
        // Parameters.
        QString correlation_file;
        QString retention_file;
        bool is_passive(false);

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
              else if (name == "passive")
                is_passive = config::parser::parse_boolean(elem.text());
            }
          }
        }

        // File exists, load it.
        if (!correlation_file.isEmpty()) {
          // Create and register correlation object.
          misc::shared_ptr<correlation::correlator>
            crltr(new correlation::correlator(cfg.instance_id(), is_passive));
          try {
            crltr->load(correlation_file, retention_file);
            correlation::obj = crltr;
            multiplexing::engine::instance().hook(*correlation::obj);
            loaded = true;
          }
          catch (std::exception const& e) {
            logging::config(logging::high) << "correlation: "
              "configuration loading error: " << e.what();
          }
          catch (...) {
            logging::config(logging::high) << "correlation: "
              "configuration loading error";
          }
        }
      }
      if (!loaded)
        logging::config(logging::high) << "correlation: invalid "
          "correlation configuration, correlation engine is NOT loaded";
    }
    return ;
  }
}
CCB_END()

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    correlation::module_deinit();
    return ;
  }

  /**
   *  Module initialization routine redirector.
   *
   *  @param[in] arg Configuration argument.
   */
  void broker_module_init(void const* arg) {
    correlation::module_init(arg);
    return ;
  }

  /**
   *  Module update routine.
   *
   *  @param[in] arg Configuration argument.
   */
  void broker_module_update(void const* arg) {
    if (!correlation::obj.isNull())
      correlation::obj->stopping();
    correlation::module_deinit();
    correlation::module_init(arg);
    if (!correlation::obj.isNull())
      correlation::obj->starting();
    return ;
  }
}
