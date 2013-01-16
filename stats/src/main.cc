/*
** Copyright 2012 Merethis
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

#include <cerrno>
#include <cstring>
#include <memory>
#include <QDomDocument>
#include <QDomElement>
#include <sys/stat.h>
#include <unistd.h>
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/stats/worker.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

// Worker.
static std::auto_ptr<stats::worker> obj;

extern "C" {
  /**
   * Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances && obj.get()) {
      // Terminate thread.
      obj->exit();
      obj->wait();
      obj.reset();
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
      // Check that stats are enabled.
      config::state const& cfg(*static_cast<config::state const*>(arg));
      bool loaded(false);
      QMap<QString, QString>::const_iterator
        it(cfg.params().find("stats"));
      if (it != cfg.params().end()) {
        // Parameters.
        QString fifo_file;

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
              if (name == "fifo")
                fifo_file = elem.text();
            }
          }
        }

        // File configured, load stats engine.
        if (!fifo_file.isEmpty()) {
          try {
            // Does file exist and is a FIFO ?
            struct stat s;
            // Stat failed, probably because of inexistant file.
            if (stat(qPrintable(fifo_file), &s) != 0) {
              char const* msg(strerror(errno));
              logging::config(logging::medium) << "stats: cannot stat() '"
                << fifo_file << "': " << msg;

              // Create FIFO.
              if (mkfifo(
                    qPrintable(fifo_file),
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != 0) {
                char const* msg(strerror(errno));
                throw (exceptions::msg() << "cannot create FIFO '"
                       << fifo_file << "': " << msg);
              }
            }
            else if (!S_ISFIFO(s.st_mode))
              throw (exceptions::msg() <<  "file '" << fifo_file
                     << "' exists but is not a FIFO");

            // Create thread.
            obj.reset(new stats::worker);
            obj->run(fifo_file);
          }
          catch (std::exception const& e) {
            logging::config(logging::high) << "stats: "
              "engine loading failure: " << e.what();
          }
          catch (...) {
            logging::config(logging::high) << "stats: "
              "engine loading failure";
          }
        }
      }
      if (!loaded)
        logging::config(logging::high) << "stats: invalid stats " \
          "configuration, stats engine is NOT loaded";
    }
    return ;
  }
}
