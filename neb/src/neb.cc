/*
** Copyright 2009-2012 Merethis
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

#include <QCoreApplication>
#include <QTimer>
#include <stddef.h>
#include <string.h>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/nebcallbacks.hh"

using namespace com::centreon::broker;

// Specify the event broker API version.
NEB_API_VERSION(CURRENT_NEB_API_VERSION)

// Centreon Engine/Nagios function.
extern "C" {
  extern timed_event* event_list_high;
  extern timed_event* event_list_high_tail;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Qt initialization flag.
static bool        gl_initialized_qt(false);
static int         gl_qt_argc;
static char const* gl_qt_argv[2] = {
  "CentreonBrokerModule",
  NULL
};

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Process Qt events.
 *
 *  @param[in] arg Unused.
 */
static void process_qcore(void* arg) {
  (void)arg;
  QCoreApplication* app(QCoreApplication::instance());
  QTimer::singleShot(0, app, SLOT(quit()));
  app->exec();
  return ;
}

/**************************************
*                                     *
*         Exported Functions          *
*                                     *
**************************************/

extern "C" {
  /**
   *  @brief Module exit point.
   *
   *  This function is called when the module gets unloaded by Nagios.
   *  It will deregister all previously registered callbacks and perform
   *  some shutdown stuff.
   *
   *  @param[in] flags  Informational flags.
   *  @param[in] reason Unload reason.
   *
   *  @return 0 on success, any other value on failure.
   */
  int nebmodule_deinit(int flags, int reason) {
    (void)flags;
    (void)reason;

    try {
      // Unregister callbacks.
      neb::unregister_callbacks();

      // Unload singletons.
      config::applier::endpoint::instance().unload();
      config::applier::modules::instance().unload();

      // Deregister Qt application object.
      if (gl_initialized_qt) {
        timed_event* te(NULL);
        for (timed_event* current = event_list_high;
             current != event_list_high_tail;
             current = current->next)
          if (current->event_data == (void*)process_qcore) {
            te = current;
            break ;
          }
        if (te)
          remove_event(te, &event_list_high, &event_list_high_tail);
        delete [] QCoreApplication::instance();
      }
    }
    // Avoid exception propagation in C code.
    catch (...) {}

    return (0);
  }

  /**
   *  @brief Module entry point.
   *
   *  This function is called when the module gets loaded by Nagios. It
   *  will register callbacks to catch events and perform some
   *  initialization stuff like config file parsing, thread creation,
   *  ...
   *
   *  @param[in] flags  Informational flags.
   *  @param[in] args   The argument string of the module (shall contain the
   *                    configuration file name).
   *  @param[in] handle The module handle.
   *
   *  @return 0 on success, any other value on failure.
   */
  int nebmodule_init(int flags, char const* args, void* handle) {
    // Initialization.
    config::applier::init();

    // Save module handle and flags for future use.
    neb::gl_flags = flags;
    neb::gl_mod_handle = handle;

    // Set module informations.
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_TITLE,
      "CentreonBroker's cbmod");
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_AUTHOR,
      "Merethis");
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_COPYRIGHT,
      "Copyright 2009-2012 Merethis");
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_VERSION,
      CENTREON_BROKER_VERSION);
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_LICENSE,
      "GPL version 2");
    neb_set_module_info(
      neb::gl_mod_handle,
      NEBMODULE_MODINFO_DESC,
      "cbmod is part of CentreonBroker and is designed to " \
      "convert internal Nagios events to a proper data "    \
      "stream that can then be parsed by CentreonBroker's " \
      "cbd.");

    // Initialize Qt if not already done by parent process.
    if (!QCoreApplication::instance()) {
      gl_initialized_qt = true;
      new QCoreApplication(gl_qt_argc, (char**)gl_qt_argv);
    }

    // Disable timestamp printing in logs (cause starvation when forking).
    logging::file::with_timestamp(false);

    try {
      // Set configuration file.
      if (args) {
        char const* config_file("config_file=");
        size_t config_file_size(strlen(config_file));
        if (!strncmp(args, config_file, config_file_size))
          args += config_file_size;
        neb::gl_configuration_file = args;
      }
      else
        throw (exceptions::msg()
                 << "main: no configuration file provided");

      // Default logging object.
      {
        config::logger default_log;
        default_log.type(config::logger::monitoring);
        default_log.config(true);
        default_log.debug(false);
        default_log.error(true);
        default_log.info(false);
        default_log.level(logging::high);
        QList<config::logger> default_logs;
        default_logs.push_back(default_log);
        config::applier::logger::instance().apply(default_logs);
      }

      // Try configuration parsing.
      config::parser p;
      config::state s;
      p.parse(neb::gl_configuration_file, s);

      // Apply loggers.
      config::applier::logger::instance().apply(s.loggers());
    }
    catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
      return (-1);
    }
    catch (...) {
      logging::error(logging::high)
        << "main: configuration file parsing failed";
      return (-1);
    }

    // Register process callback.
    if (neb_register_callback(
          NEBCALLBACK_PROCESS_DATA,
          neb::gl_mod_handle,
          0,
          &neb::callback_process) != OK)
      return (-1);

    // Process Qt events if necessary.
    if (gl_initialized_qt)
      schedule_new_event(
        99,
        1,
        time(NULL) + 1,
        1,
        1,
        NULL,
        1,
        (void*)process_qcore,
        NULL,
        0);

    return (0);
  }
}
