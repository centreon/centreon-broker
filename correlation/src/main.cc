/*
** Copyright 2011-2014 Centreon
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

#include <QDomDocument>
#include <QDomElement>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/internal.hh"
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

      // Remove elements.
      io::events::instance().unreg("correlation");
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

      // Load elements.
      {
        std::set<unsigned int> elements;
        elements.insert(
                   io::events::data_type<io::events::correlation, correlation::de_engine_state>::value);
        elements.insert(
                   io::events::data_type<io::events::correlation, correlation::de_host_state>::value);
        elements.insert(
                   io::events::data_type<io::events::correlation, correlation::de_issue>::value);
        elements.insert(
                   io::events::data_type<io::events::correlation, correlation::de_issue_parent>::value);
        elements.insert(
                   io::events::data_type<io::events::correlation, correlation::de_service_state>::value);
        io::events::instance().reg("correlation", elements);
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
