/*
** Copyright 2013,2015 Centreon
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

#include "com/centreon/broker/misc/diagnostic.hh"
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <thread>
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
diagnostic::diagnostic() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
diagnostic::diagnostic(diagnostic const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
diagnostic::~diagnostic() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
diagnostic& diagnostic::operator=(diagnostic const& right) {
  (void)right;
  return (*this);
}

/**
 *  Generate diagnostic file.
 *
 *  @param[in]  cfg_files Main configuration files.
 *  @param[out] out_file  Output file.
 */
void diagnostic::generate(std::vector<std::string> const& cfg_files,
                          std::string const& out_file) {
  // Destination directory.
  std::string tmp_dir;
  {
    tmp_dir = temp_path();
    std::ifstream dir(tmp_dir);
    if (!dir.good())
      throw exceptions::msg() << "diagnostic: cannot create temporary file";
  }

  // Files to remove.
  std::list<std::string> to_remove;

  // Add diagnostic log file.
  config::state diagnostic_state;
  {
    std::string diagnostic_log_path;
    diagnostic_log_path = tmp_dir;
    diagnostic_log_path.append("/diagnostic.log");
    to_remove.push_back(diagnostic_log_path);
    {
      config::logger diagnostic_log;
      diagnostic_log.config(true);
      diagnostic_log.debug(true);
      diagnostic_log.error(true);
      diagnostic_log.info(true);
      diagnostic_log.level(logging::low);
      diagnostic_log.name(diagnostic_log_path.c_str());
      diagnostic_log.type(config::logger::file);
      diagnostic_state.loggers().push_back(diagnostic_log);
    }
    {
      config::logger stdout_log;
      stdout_log.config(false);
      stdout_log.debug(false);
      stdout_log.error(true);
      stdout_log.info(true);
      stdout_log.level(logging::high);
      stdout_log.name("stdout");
      stdout_log.type(config::logger::standard);
      diagnostic_state.loggers().push_back(stdout_log);
    }
  }
  config::applier::logger::instance().apply(diagnostic_state.loggers());

  // Base information about the software.
  logging::info(logging::high)
      << "diagnostic: Centreon Broker " << CENTREON_BROKER_VERSION;

  // df.
  logging::info(logging::high) << "diagnostic: getting disk usage";
  {
    std::string df_log_path;
    df_log_path = tmp_dir;
    df_log_path.append("/df.log");
    to_remove.push_back(df_log_path);
    std::string output{misc::exec("df -P")};

    std::ofstream out(df_log_path);
    out << output;
    out.close();
  }

  // lsb_release.
  logging::info(logging::high) << "diagnostic: getting LSB information";
  {
    std::string lsb_release_log_path;
    lsb_release_log_path = tmp_dir;
    lsb_release_log_path.append("/lsb_release.log");
    to_remove.push_back(lsb_release_log_path);
    std::string output{misc::exec("lsb_release -a")};

    std::ofstream out(lsb_release_log_path);
    out << output;
    out.close();
  }

  // uname.
  logging::info(logging::high) << "diagnostic: getting system name";
  {
    std::string uname_log_path;
    uname_log_path = tmp_dir;
    uname_log_path.append("/uname.log");
    to_remove.push_back(uname_log_path);
    std::string output{misc::exec("uname -a")};

    std::ofstream out(uname_log_path);
    out << output;
    out.close();
  }

  // /proc/version
  logging::info(logging::high) << "diagnostic: getting kernel information";
  {
    std::string proc_version_log_path;
    proc_version_log_path = tmp_dir;
    proc_version_log_path.append("/proc_version.log");
    to_remove.push_back(proc_version_log_path);
    std::string output{misc::exec("cat /proc/version")};

    std::ofstream out(proc_version_log_path);
    out << output;
    out.close();
  }

  // netstat.
  logging::info(logging::high)
      << "diagnostic: getting network connections information";
  {
    std::string netstat_log_path;
    netstat_log_path = tmp_dir;
    netstat_log_path.append("/netstat.log");
    to_remove.push_back(netstat_log_path);
    std::string output{misc::exec("netstat -ap --numeric-hosts")};

    std::ofstream out(netstat_log_path);
    out << output;
    out.close();
  }

  // ps.
  logging::info(logging::high) << "diagnostic: getting processes information";
  {
    std::string ps_log_path;
    ps_log_path = tmp_dir;
    ps_log_path.append("/ps.log");
    to_remove.push_back(ps_log_path);
    std::string output{misc::exec("ps aux")};

    std::ofstream out(ps_log_path);
    out << output;
    out.close();
  }

  // rpm.
  logging::info(logging::high) << "diagnostic: getting packages information";
  {
    std::string rpm_log_path;
    rpm_log_path = tmp_dir;
    rpm_log_path.append("/rpm.log");
    to_remove.push_back(rpm_log_path);
    std::string output{misc::exec("rpm -qa centreon")};

    std::ofstream out(rpm_log_path);
    out << output;
    out.close();
  }

  // sestatus.
  logging::info(logging::high) << "diagnostic: getting SELinux status";
  {
    std::string selinux_log_path;
    selinux_log_path = tmp_dir;
    selinux_log_path.append("/selinux.log");
    to_remove.push_back(selinux_log_path);
    std::string output{misc::exec("sestatus")};

    std::ofstream out(selinux_log_path);
    out << output;
    out.close();
  }

  // Browse configuration files.
  for (std::vector<std::string>::const_iterator it(cfg_files.begin()),
       end(cfg_files.end());
       it != end; ++it) {
    // Configuration file.
    logging::info(logging::high)
        << "diagnostic: getting configuration file '" << *it << "'";
    std::string cfg_path;
    {
      cfg_path = tmp_dir;
      cfg_path.append("/");
      size_t pos(it->find_last_of('/'));
      if (pos != std::string::npos)
        cfg_path.append(it->substr(pos + 1));
      else
        cfg_path.append(*it);
      to_remove.push_back(cfg_path);
      std::ostringstream oss;
      oss << "cp " << *it << " " << cfg_path;
      misc::exec(oss.str());
    }

    // Parse configuration file.
    config::parser parsr;
    config::state conf;
    try {
      parsr.parse(it->c_str(), conf);
    } catch (std::exception const& e) {
      logging::error(logging::high) << "diagnostic: configuration file '" << *it
                                    << "' parsing failed: " << e.what();
    }

    // ls.
    logging::info(logging::high)
        << "diagnostic:     getting modules information";
    {
      std::string ls_log_path;
      ls_log_path = tmp_dir;
      ls_log_path.append("/ls_modules_");
      size_t pos(it->find_last_of('/'));
      if (pos != std::string::npos)
        ls_log_path.append(it->substr(pos + 1));
      else
        ls_log_path.append(*it);
      ls_log_path.append(".log");
      to_remove.push_back(ls_log_path);
      std::ostringstream oss;
      oss << "ls -la " << conf.module_directory();
      for (std::list<std::string>::const_iterator
               it(conf.module_list().begin()),
           end(conf.module_list().end());
           it != end; ++it)
        oss << " " << *it;
      std::string output{misc::exec(oss.str())};

      std::ofstream out(ls_log_path);
      out << output;
      out.close();
    }

    // Log files.
    logging::info(logging::high) << "diagnostic:     getting log files";
    for (std::list<config::logger>::const_iterator it(conf.loggers().begin()),
         end(conf.loggers().end());
         it != end; ++it)
      if (it->type() == config::logger::file) {
        std::string log_path;
        log_path = tmp_dir;
        log_path.append("/");
        size_t pos{it->name().find_last_of('/')};
        if (pos != std::string::npos)
          log_path.append(it->name().substr(pos));
        else
          log_path.append(it->name());
        to_remove.push_back(log_path);

        char const* args[]{"tail", "-c", "20000000", it->name().c_str(), nullptr};
        misc::exec_process(args, true);
      }
  }

  // Generate file name if not existing.
  std::string my_out_file;
  if (out_file.empty())
    my_out_file = "cbd-diag.tar.gz";
  else
    my_out_file = out_file;

  // Create tarball.
  logging::info(logging::high)
      << "diagnostic: creating tarball '" << my_out_file << "'";
  {
    std::ostringstream oss;
    oss << "tar "
        << "czf " << my_out_file << " " << tmp_dir;
    std::string output{misc::exec(oss.str())};
  }

  // Clean temporary directory.
  {
    for (std::list<std::string>::const_iterator it(to_remove.begin()),
         end(to_remove.end());
         it != end; ++it)
      ::remove(it->c_str());
    ::rmdir(tmp_dir.c_str());
  }

  return;
}
