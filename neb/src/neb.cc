/*
** Copyright 2009-2013,2015-2016,2018 Centreon
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

#include <clocale>
#include <csignal>
#include <cstring>
#include <QCoreApplication>
#include <QTextCodec>
#include <QTimer>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/instance_configuration.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/monitoring_logger.hh"
#include "com/centreon/broker/neb/engcmd/internal.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/events/defines.hh"
#include "com/centreon/engine/events/timed_event.hh"
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
      com::centreon::broker::config::applier::deinit();

      // Deregister Qt application object.
      if (gl_initialized_qt) {
        timed_event* te(NULL);
        for (timed_event* current = event_list_high;
             current != event_list_high_tail;
             current = current->next) {
          union {
            void (* code)(void*);
            void *  data;
          } val;
          val.code = &process_qcore;
          if (current->event_data == val.data) {
            te = current;
            break ;
          }
        }
        if (te)
          remove_event(te, &event_list_high, &event_list_high_tail);
        delete QCoreApplication::instance();
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
    try {
      // Initialization.
      com::centreon::broker::config::applier::init();

      // Initialize the engcmd module.
      ::com::centreon::broker::neb::engcmd::load();

      // Save module handle and flags for future use.
      neb::gl_mod_flags = flags;
      neb::gl_mod_handle = handle;

      // Set module informations.
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_TITLE,
        "Centreon Broker's cbmod");
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_AUTHOR,
        "Centreon");
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_COPYRIGHT,
        "Copyright 2009-2018 Centreon");
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_VERSION,
        CENTREON_BROKER_VERSION);
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_LICENSE,
        "ASL 2.0");
      neb_set_module_info(
        neb::gl_mod_handle,
        NEBMODULE_MODINFO_DESC,
        "cbmod is part of Centreon Broker and is designed to "    \
        "convert internal Centreon Engine events to a "  \
        "proper data stream that can then be parsed by Centreon " \
        "Broker's cbd.");

      // Initialize Qt if not already done by parent process.
      if (!QCoreApplication::instance()) {
        gl_initialized_qt = true;
        new QCoreApplication(gl_qt_argc, (char**)gl_qt_argv);
        signal(SIGCHLD, SIG_DFL);
        QTextCodec* utf8_codec(QTextCodec::codecForName("UTF-8"));
        if (utf8_codec)
          QTextCodec::setCodecForCStrings(utf8_codec);
        else
          logging::error(logging::high)
            << "core: could not find UTF-8 codec, strings will be "
               "interpreted using the current locale";
      }
      // Qt already loaded.
      else
        logging::info(logging::high)
          << "core: Qt was already loaded";

      // Reset locale.
      setlocale(LC_NUMERIC, "C");

      // Default logging object.
      neb::monitoring_logger monlog;

      try {
        // Default logging object.
        {
          // Debug ?
          bool debug;
          char const* dbg_flag("-d ");
          if (args && !strncmp(args, dbg_flag, strlen(dbg_flag))) {
            debug = true;
            args += strlen(dbg_flag);
          }
          else
            debug = false;

          // Add log.
          logging::manager::instance().log_on(
                                         monlog,
                                         (debug
                                          ? logging::config_type
                                          | logging::debug_type
                                          | logging::error_type
                                          | logging::info_type
                                          : logging::config_type
                                          | logging::error_type
                                          | logging::info_type),
                                         (debug ?
                                          logging::low
                                          : logging::high));
        }

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

        // Try configuration parsing.
	com::centreon::broker::config::parser p;
	com::centreon::broker::config::state s;
        p.parse(neb::gl_configuration_file, s);

        // Apply loggers.
	com::centreon::broker::config::applier::logger::instance().apply(
          s.loggers());

        // Remove monitoring log.
        logging::manager::instance().log_on(monlog, 0);
      }
      catch (std::exception const& e) {
        logging::error(logging::high) << e.what();
        logging::manager::instance().log_on(monlog, 0);
        return (-1);
      }
      catch (...) {
        logging::error(logging::high)
          << "main: configuration file parsing failed";
        logging::manager::instance().log_on(monlog, 0);
        return (-1);
      }

      // Remove old monitoring object.
      logging::manager::instance().log_on(monlog, 0);

      // Register process and log callback.
      neb::gl_registered_callbacks.push_back(
             std::shared_ptr<neb::callback>(
               new neb::callback(
                          NEBCALLBACK_PROCESS_DATA,
                          neb::gl_mod_handle,
                          &neb::callback_process)));
      neb::gl_registered_callbacks.push_back(
             std::shared_ptr<neb::callback>(
               new neb::callback(
                          NEBCALLBACK_LOG_DATA,
                          neb::gl_mod_handle,
                          &neb::callback_log)));

      // Process Qt events if necessary.
      if (gl_initialized_qt) {
        union {
          void (* code)(void*);
          void*   data;
        } val;
        val.code = &process_qcore;
        schedule_new_event(
          EVENT_USER_FUNCTION,
          1,
          time(NULL) + 1,
          1,
          1,
          NULL,
          1,
          val.data,
          NULL,
          0);
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "main: cbmod loading failed: " << e.what();
      nebmodule_deinit(0, 0);
      return (-1);
    }
    catch (...) {
      logging::error(logging::high)
        << "main: cbmod loading failed due to an unknown exception";
      nebmodule_deinit(0, 0);
      return (-1);
    }

    return (0);
  }

  /**
   *  @brief Reload module after configuration reload.
   *
   *  This will effectively send an instance_configuration object to the
   *  multiplexer.
   *
   *  @return OK.
   */
  int nebmodule_reload() {
    std::shared_ptr<neb::instance_configuration>
      ic(new neb::instance_configuration);
    ic->loaded = true;
    ic->poller_id = config::applier::state::instance().poller_id();
    multiplexing::publisher p;
    p.write(ic);
    return (0);
  }
}
