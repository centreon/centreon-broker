/*
** Copyright 2013 Merethis
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

#include <cstdlib>
#include <QDir>
#include <QLibraryInfo>
#include <QProcess>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/diagnostic.hh"

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
diagnostic::~diagnostic() throw () {}

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
 *  @param[in]  cfg_file Main configuration file.
 *  @param[out] out_file Output file.
 */
void diagnostic::generate(
                   std::string const& cfg_file,
                   std::string const& out_file) {
  // Destination directory.
  std::string tmp_dir;
  {
    char const* tmp_dir_ptr(tmpnam(NULL));
    if (!tmp_dir_ptr)
      throw (exceptions::msg()
             << "diagnostic: cannot generate temporary directory path");
    tmp_dir = tmp_dir_ptr;
    QDir dir;
    if (!dir.mkdir(tmp_dir.c_str()))
      throw (exceptions::msg()
             << "diagnostic: cannot create temporary directory path");
  }

  // Add diagnostic log file.
  std::string diagnostic_log_path;
  config::state diagnostic_state;
  {
    diagnostic_log_path = tmp_dir;
    diagnostic_log_path.append("/diagnostic.log");
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
  config::applier::state::instance().apply(diagnostic_state, false);

  // Base information about the software.
  logging::info(logging::high)
    << "diagnostic: Centreon Broker " << CENTREON_BROKER_VERSION;
  logging::info(logging::high) << "diagnostic: using Qt " << qVersion()
    << " " << QLibraryInfo::buildKey()
    << " (compiled with " << QT_VERSION_STR << ")";

  // df.
  logging::info(logging::high) << "diagnostic: getting disk usage";
  std::string df_log_path;
  {
    df_log_path = tmp_dir;
    df_log_path.append("/df.log");
    QProcess p;
    p.setStandardOutputFile(df_log_path.c_str());
    p.start("df -P");
    p.waitForFinished();
  }

  // lsb_release.
  logging::info(logging::high) << "diagnostic: getting LSB information";
  std::string lsb_release_log_path;
  {
    lsb_release_log_path = tmp_dir;
    lsb_release_log_path.append("/lsb_release.log");
    QProcess p;
    p.setStandardOutputFile(lsb_release_log_path.c_str());
    p.start("lsb_release -a");
    p.waitForFinished();
  }

  // uname.
  logging::info(logging::high) << "diagnostic: getting system name";
  std::string uname_log_path;
  {
    uname_log_path = tmp_dir;
    uname_log_path.append("/uname.log");
    QProcess p;
    p.setStandardOutputFile(uname_log_path.c_str());
    p.start("uname -a");
    p.waitForFinished();
  }

  // /proc/version
  logging::info(logging::high)
    << "diagnostic: getting kernel information";
  std::string proc_version_log_path;
  {
    proc_version_log_path = tmp_dir;
    proc_version_log_path.append("/proc_version.log");
    QProcess p;
    p.setStandardOutputFile(proc_version_log_path.c_str());
    p.start("cat /proc/version");
    p.waitForFinished();
  }

  // netstat.
  logging::info(logging::high)
    << "diagnostic: getting network connections information";
  std::string netstat_log_path;
  {
    netstat_log_path = tmp_dir;
    netstat_log_path.append("/netstat.log");
    QProcess p;
    p.setStandardOutputFile(netstat_log_path.c_str());
    p.start("netstat -ap --numeric-hosts");
    p.waitForFinished();
  }

  // ps.
  logging::info(logging::high)
    << "diagnostic: getting processes information";
  std::string ps_log_path;
  {
    ps_log_path = tmp_dir;
    ps_log_path.append("/ps.log");
    QProcess p;
    p.setStandardOutputFile(ps_log_path.c_str());
    p.start("ps aux");
    p.waitForFinished();
  }

  // rpm.
  logging::info(logging::high)
    << "diagnostic: getting packages information";
  std::string rpm_log_path;
  {
    rpm_log_path = tmp_dir;
    rpm_log_path.append("/rpm.log");
    QStringList args;
    args.push_back("-qa");
    args.push_back("centreon*");
    QProcess p;
    p.setStandardOutputFile(rpm_log_path.c_str());
    p.start("rpm", args);
    p.waitForFinished();
  }

  // selinuxenabled.
  logging::info(logging::high)
    << "diagnostic: getting SELinux status";
  std::string selinux_log_path;
  {
    selinux_log_path = tmp_dir;
    selinux_log_path.append("/selinux.log");
    QProcess p;
    p.setStandardOutputFile(selinux_log_path.c_str());
    p.start("sestatus");
    p.waitForFinished();
  }

  // Configuration file.
  logging::info(logging::high)
    << "diagnostic: getting configuration file";
  std::string cfg_path;
  {
    cfg_path = tmp_dir;
    cfg_path.append("/cbd.xml");
    QStringList args;
    args.push_back(cfg_file.c_str());
    args.push_back(cfg_path.c_str());
    QProcess p;
    p.start("cp", args);
    p.waitForFinished();
  }

  // Parse configuration file.
  config::parser parsr;
  config::state conf;
  try {
    parsr.parse(cfg_file.c_str(), conf);
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "diagnostic: configuration file parsing failed: "
      << e.what();
  }

  // ls.
  logging::info(logging::high)
    << "diagnostic: getting modules information";
  std::string ls_log_path;
  {
    ls_log_path = tmp_dir;
    ls_log_path.append("ls_modules.log");
    QStringList args;
    args.push_back(conf.module_directory());
    for (QList<QString>::const_iterator
           it(conf.module_list().begin()),
           end(conf.module_list().end());
         it != end;
         ++it)
      args.push_back(*it);
    QProcess p;
    p.setStandardOutputFile(ls_log_path.c_str());
    p.start("ls", args);
    p.waitForFinished();
  }

  // Log files.
  logging::info(logging::high) << "diagnostic: getting log files";
  std::list<std::string> log_paths;
  unsigned int i(1);
  for (QList<config::logger>::const_iterator
         it(conf.loggers().begin()),
         end(conf.loggers().end());
       it != end;
       ++it, ++i)
    if (it->type() == config::logger::file) {
      std::string log_path;
      {
        std::ostringstream oss;
        oss << tmp_dir << "/cbd" << i << ".log";
        log_path = oss.str();
      }
      log_paths.push_back(log_path);
      QStringList args;
      args.push_back("-c");
      args.push_back("20000000");
      args.push_back(it->name());
      QProcess p;
      p.setStandardOutputFile(log_path.c_str());
      p.start("tail", args);
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
    QStringList args;
    args.push_back("czf");
    args.push_back(my_out_file.c_str());
    args.push_back(tmp_dir.c_str());
    QProcess p;
    p.start("tar", args);
    p.waitForFinished(-1);
  }

  // Clean temporary directory.
  {
    if (!df_log_path.empty())
      ::remove(df_log_path.c_str());
    if (!diagnostic_log_path.empty())
      ::remove(diagnostic_log_path.c_str());
    if (!lsb_release_log_path.empty())
      ::remove(lsb_release_log_path.c_str());
    if (!uname_log_path.empty())
      ::remove(uname_log_path.c_str());
    if (!proc_version_log_path.empty())
      ::remove(proc_version_log_path.c_str());
    if (!netstat_log_path.empty())
      ::remove(netstat_log_path.c_str());
    if (!ps_log_path.empty())
      ::remove(ps_log_path.c_str());
    if (!rpm_log_path.empty())
      ::remove(rpm_log_path.c_str());
    if (!selinux_log_path.empty())
      ::remove(selinux_log_path.c_str());
    if (!cfg_path.empty())
      ::remove(cfg_path.c_str());
    if (!ls_log_path.empty())
      ::remove(ls_log_path.c_str());
    for (std::list<std::string>::const_iterator
           it(log_paths.begin()),
           end(log_paths.end());
         it != end;
         ++it)
      ::remove(it->c_str());
    QDir dir;
    dir.rmdir(tmp_dir.c_str());
  }

  return ;
}
