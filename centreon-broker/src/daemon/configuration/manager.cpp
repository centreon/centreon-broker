/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <algorithm>
#include <memory>
#include <signal.h>
#include <stdlib.h>                       // for strtoul
#include "concurrency/thread.h"
#include "configuration/lexer.h"
#include "configuration/interface.h"
#include "configuration/manager.h"
#include "db/mysql/connection.h"
#include "exception.h"
#include "interface/db/destination.h"
#include "interface/factory.h"
#include "io/acceptor.h"
#include "io/file.h"
#include "io/net/ipv4.h"
#include "io/net/ipv6.h"
#include "io/net/unix.h"
#include "logging.h"
#include "processing/high_availability.h"
#include "processing/listener.h"

#define INVALID_TOKEN_MSG "Invalid token encountered while parsing " \
                          "configuration file ..."

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Get the instance of the configuration manager.
 *
 *  \return The instance of the configuration manager.
 */
Configuration::Manager& Configuration::Manager::Instance()
{
  static Configuration::Manager manager;

  return (manager);
}

/**
 *  Process configuration options of an Interface.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] i     Object that will be set with extracted parameters.
 */
static void HandleInterface(Configuration::Lexer& lexer,
                            Configuration::Interface& i)
{
  Configuration::Token var;

  LOGDEBUG("Interface definition ...");
  for (lexer.GetToken(var);
       var.GetType() == Configuration::Token::STRING;
       lexer.GetToken(var))
    {
      Configuration::Token val;

      // Check if the current token is valid (var name).
      if (var.GetType() != Configuration::Token::STRING
          // And the next one too (=).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::ASSIGNMENT)
          // And the next-next one too (var value).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
        throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string& var_str = var.GetText();
      const std::string& val_str = val.GetText();

      // Parse variable.
      if (var_str == "db")
        i.db = val_str;
      else if (var_str == "host")
        {
          i.host = val_str;
          if (Configuration::Interface::IPV4_SERVER == i.type)
            i.type = Configuration::Interface::IPV4_CLIENT;
          else if (Configuration::Interface::IPV6_SERVER == i.type)
            i.type = Configuration::Interface::IPV6_CLIENT;
          else if (Configuration::Interface::UNIX_SERVER == i.type)
            i.type = Configuration::Interface::UNIX_CLIENT;
        }
      else if (var_str == "interface")
        i.interface = val_str;
      else if (var_str == "password")
        i.password = val_str;
      else if (var_str == "port")
        i.port = strtoul(val_str.c_str(), NULL, 0);
      else if (var_str == "protocol")
        i.protocol = ((val_str == "xml") ? Configuration::Interface::XML
                                         : Configuration::Interface::NDO);
      else if (var_str == "socket")
        i.socket = val_str;
      else if (var_str == "user")
        i.user = val_str;
      else if (var_str == "type")
        {
          if ((val_str == "ip") || (val_str == "ipv4"))
            i.type = (i.host.empty() ? Configuration::Interface::IPV4_SERVER
                                     : Configuration::Interface::IPV4_CLIENT);
          else if (val_str == "ipv6")
            i.type = (i.host.empty() ? Configuration::Interface::IPV6_SERVER
                                     : Configuration::Interface::IPV6_CLIENT);
          else if (val_str == "mysql")
            i.type = Configuration::Interface::MYSQL;
          else if (val_str == "oracle")
            i.type = Configuration::Interface::ORACLE;
          else if (val_str == "postgresql")
            i.type = Configuration::Interface::POSTGRESQL;
          else if (val_str == "unix")
            i.type = (i.host.empty() ? Configuration::Interface::UNIX_SERVER
                                     : Configuration::Interface::UNIX_CLIENT);
        }
#ifdef USE_TLS
      else if (var_str == "ca")
        i.ca = val_str;
      else if (var_str == "cert")
        i.cert = val_str;
      else if (var_str == "compress")
        i.compress = strtoul(val_str.c_str(), NULL, 0);
      else if (var_str == "key")
        i.key = val_str;
      else if (var_str == "tls")
        i.tls = ((val_str == "yes") || strtoul(val_str.c_str(), NULL, 0));
#endif /* USE_TLS */
    }
  return ;
}

/**
 *  Process configuration options of a Log.
 *
 *  \param[in]  lexer Lexer of the configuration file.
 *  \param[out] log   Object that will be set with extracted parameters.
 */
static void HandleLog(Configuration::Lexer& lexer, Configuration::Log& log)
{
  LOGDEBUG("Log definition ...");
  Configuration::Token var;

  for (lexer.GetToken(var);
       var.GetType() != Configuration::Token::END && var.GetType() != Configuration::Token::BLOCK_END;
       lexer.GetToken(var))
    {
      Configuration::Token val;

      // Check if the current token is valid (var name).
      if ((var.GetType() != Configuration::Token::STRING)
          // And the next one too (=).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::ASSIGNMENT)
          // And the next-next one too (var value).
          || lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
        throw (Exception(0, INVALID_TOKEN_MSG));

      // Extract var strings.
      const std::string var_str = var.GetText();
      const std::string val_str = val.GetText();

      // Parse variable.
      if (var_str == "flags")
        {
          unsigned int flags;

          flags = 0;

          // We will break when there's no more pipes.
          do
            {
              if (val.GetText() == "DEBUG")
                flags |= CentreonBroker::Logging::DEBUG;
              else if (val.GetText() == "ERROR")
                flags |= CentreonBroker::Logging::ERROR;
              else if (val.GetText() == "INFO")
                flags |= CentreonBroker::Logging::INFO;
              lexer.ContextSave();
              if (lexer.GetToken(val)
                  || (val.GetType() != Configuration::Token::PIPE)
                  || lexer.GetToken(val)
                  || (val.GetType() != Configuration::Token::STRING))
                {
                  lexer.ContextRestore();
                  val.SetType(Configuration::Token::UNKNOWN);
                }
              else
                lexer.ContextPop();
            } while (val.GetType() == Configuration::Token::STRING);
          log.flags = flags;
        }
      else if (var_str == "path")
        log.file = val_str;
      else if (var_str == "type")
        {
          if (val_str == "file")
            log.type = Configuration::Log::FILE;
          else if (val_str == "stderr")
            log.type = Configuration::Log::STDERR;
          else if (val_str == "stdout")
            log.type = Configuration::Log::STDOUT;
          else if (val_str == "syslog")
            log.type = Configuration::Log::SYSLOG;
        }
    }
  return ;
}

/**
 *  Helper function to update configuration. This method is called whenever
 *  SIGHUP is received.
 */
static void UpdateHelper(int signum)
{
  (void)signum;
  signal(SIGHUP, SIG_IGN);
  LOGINFO("Configuration file update requested...");
  LOGINFO("  WARNING: this feature is still experimental.");
  Configuration::Manager::Instance().Update();
  signal(SIGHUP, UpdateHelper);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Manager default constructor.
 */
Configuration::Manager::Manager() {}

/**
 *  \brief Manager copy constructor.
 *
 *  Open the file stored within the given object, parse it and take actions as
 *  it would normally be done with Open().
 *
 *  \param[in] manager Object containing file to open again.
 *
 *  \see Open
 */
Configuration::Manager::Manager(const Configuration::Manager& manager)
{
  this->operator=(manager);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Open the file stored within the given object, parse it and take actions as
 *  it would normally be done with Open().
 *
 *  \param[in] manager Object containin the file to open again.
 *
 *  \see Open
 */
Configuration::Manager& Configuration::Manager::operator=(const Configuration::Manager& manager)
{
  this->Open(manager.filename_);
  return (*this);
}

/**
 *  \brief Manager destructor.
 *
 *  Delete all objects created by the file parsing.
 */
Configuration::Manager::~Manager()
{
  this->Close();
}

/**
 *  Parse and analyze the configuration file and store resulting configuration
 *  in appropriate objects.
 *
 *  \param[out] inputs  Object where inputs configurations will be stored.
 *  \param[out] logs    Object where logs configurations will be stored.
 *  \param[out] outputs Object where outputs configurations will be stored.
 */
void Configuration::Manager::Analyze(std::list<Configuration::Interface>& inputs,
                                     std::list<Configuration::Log>& logs,
                                     std::list<Configuration::Interface>& outputs)
{
  std::auto_ptr<IO::File> filestream(new IO::File);
  Configuration::Token val;
  Configuration::Token var;

  filestream->Open(this->filename_.c_str(), IO::File::READ);

  Configuration::Lexer lexer(filestream.get());

  filestream.release();
  for (lexer.GetToken(var), lexer.GetToken(val);
       var.GetType() == Configuration::Token::STRING;
       lexer.GetToken(var), lexer.GetToken(val))
    {
      switch (val.GetType())
        {
         // Assignment sign, we're setting a variable.
        case Configuration::Token::ASSIGNMENT:
          if (lexer.GetToken(val) || (val.GetType() != Configuration::Token::STRING))
            throw (Exception(0, INVALID_TOKEN_MSG));
          // XXX : set global variable
          break ;
         // Block name, can safely be discarded.
        case Configuration::Token::STRING:
           if (lexer.GetToken(val) || val.GetType() != Configuration::Token::BLOCK_START)
            throw (Exception(0, INVALID_TOKEN_MSG));
         // Starting a bloc, launching proper handler.
        case Configuration::Token::BLOCK_START:
          if (var.GetText() == "input")
            {
              Configuration::Interface in;

              HandleInterface(lexer, in);
              inputs.push_back(in);
            }
          else if (var.GetText() == "log")
            {
              Configuration::Log log;

              HandleLog(lexer, log);
              logs.push_back(log);
            }
          else if (var.GetText() == "output")
            {
              Configuration::Interface out;

              HandleInterface(lexer, out);
              outputs.push_back(out);
            }
          else
            throw (Exception(0, INVALID_TOKEN_MSG));
          break ;
         // Invalid token.
         default:
          throw (Exception(0, INVALID_TOKEN_MSG));
        };
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Delete all objects created by the file parsing.
 */
void Configuration::Manager::Close()
{
  LOGDEBUG("Closing configuration manager...");

  LOGDEBUG("Closing input objects...");
  for (std::map<Configuration::Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();
       it++)
    delete (it->second);
  this->inputs_.clear();

  LOGDEBUG("Closing output objects...");
  for (std::map<Configuration::Interface, Processing::HighAvailability*>::iterator
         it = this->outputs_.begin();
       it != this->outputs_.end();
       it++)
    delete (it->second);
  this->outputs_.clear();

  // Does nothing of the log outputs. This way, they will remain valid until
  // the program exits.
  this->logs_.clear();

  return ;
}

/**
 *  Open a configuration file.
 *
 *  \param[in] filename Configuration file.
 */
void Configuration::Manager::Open(const std::string& filename)
{
  this->filename_ = filename;
  this->Update();
  return ;
}

/**
 *  Update a previously opened configuration file.
 */
void Configuration::Manager::Update()
{
  std::list<Configuration::Interface> inputs;
  std::list<Configuration::Interface>::iterator inputs_it;
  std::list<Configuration::Log> logs;
  std::list<Configuration::Log>::iterator logs_it;
  std::list<Configuration::Interface> outputs;
  std::list<Configuration::Interface>::iterator outputs_it;

  this->Analyze(inputs, logs, outputs);

  // Remove logs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::list<Configuration::Log>::iterator it = this->logs_.begin();
       it != this->logs_.end();)
    {
      logs_it = std::find(logs.begin(), logs.end(), *it);
      if (logs.end() == logs_it)
        {
          LOGDEBUG("Removing unwanted log object...");
          switch (it->type)
            {
             case Configuration::Log::FILE:
              CentreonBroker::logging.LogInFile(it->file.c_str(), 0);
              break ;
             case Configuration::Log::STDERR:
              CentreonBroker::logging.LogToStderr(0);
              break ;
             case Configuration::Log::STDOUT:
              CentreonBroker::logging.LogToStdout(0);
              break ;
             case Configuration::Log::SYSLOG:
              CentreonBroker::logging.LogInSyslog(0);
              break ;
             default:
              ;
            }
          this->logs_.erase(it++);
        }
      else
        {
          logs.erase(logs_it);
          it++;
        }
    }

  // Add new logs.
  for (logs_it = logs.begin(); logs_it != logs.end(); ++logs_it)
    {
      LOGDEBUG("Adding new logging object...");
      switch (logs_it->type)
        {
        case Configuration::Log::FILE:
          CentreonBroker::logging.LogInFile(logs_it->file.c_str(),
                                            logs_it->flags);
          break ;
        case Configuration::Log::STDERR:
          CentreonBroker::logging.LogToStderr(logs_it->flags);
          break ;
        case Configuration::Log::STDOUT:
          CentreonBroker::logging.LogToStdout(logs_it->flags);
          break ;
        case Configuration::Log::SYSLOG:
          CentreonBroker::logging.LogInSyslog(logs_it->flags);
          break ;
         default:
          ;
        }
      this->logs_.push_back(*logs_it);
    }

  // Remove outputs that are not present in conf anymore or which don't have
  // the same configuration.
  for (std::map<Configuration::Interface, Processing::HighAvailability*>::iterator it = this->outputs_.begin();
       it != this->outputs_.end();)
    {
      outputs_it = std::find(outputs.begin(), outputs.end(), it->first);
      if (outputs.end() == outputs_it)
        {
          LOGDEBUG("Removing unwanted output object...");
          delete (it->second);
          this->outputs_.erase(it++);
        }
      else
        {
          outputs.erase(outputs_it);
          it++;
        }
    }

  // Add new outputs.
  for (outputs_it = outputs.begin(); outputs_it != outputs.end(); outputs_it++)
    {
      std::auto_ptr< ::Interface::Destination > dbd;
      std::auto_ptr<Processing::HighAvailability> ha;
      const Configuration::Interface& output(*outputs_it);

      LOGDEBUG("Adding new output object...");

      /*
      dbo->SetConnectionRetryInterval(output.GetConnectionRetryInterval());
      dbo->SetDumpFile(output.GetDumpFile());
      dbo->SetQueryCommitInterval(output.GetQueryCommitInterval());
      dbo->SetTimeCommitInterval(output.GetTimeCommitInterval());
      dbo->Init(output.GetHost(),
                output.GetUser(),
                output.GetPassword(),
                output.GetDB());
      */

      dbd.reset(::Interface::Factory::Instance().Destination(output));
      ha.reset(new Processing::HighAvailability);
      ha->Init(dbd.get());
      dbd.release();
      this->outputs_[output] = ha.get();
      ha.release();
    }

  // Remove inputs that are not present in conf anymore or which don't have the
  // same configuration.
  for (std::map<Interface, Concurrency::Thread*>::iterator
         it = this->inputs_.begin();
       it != this->inputs_.end();)
    {
      inputs_it = std::find(inputs.begin(), inputs.end(), it->first);
      if (inputs.end() == inputs_it)
        {
          LOGDEBUG("Removing unwanted input object...");
          delete (it->second);
          this->inputs_.erase(it++);
        }
      else
        {
          inputs.erase(inputs_it);
          it++;
        }
    }

  // Add new inputs.
  for (inputs_it = inputs.begin(); inputs_it != inputs.end(); ++inputs_it)
    {
      LOGDEBUG("Adding new input object...");
      std::auto_ptr<IO::Acceptor> acceptor(
        ::Interface::Factory::Instance().Acceptor(*inputs_it));

      // Create the new client acceptor
      std::auto_ptr<Processing::Listener> listener(
        new Processing::Listener());

      listener->Init(acceptor.get(), Processing::Listener::NDO);
      acceptor.release();
      this->inputs_[*inputs_it] = listener.get();
      listener.release();
    }

  // (Re-)Register handler of SIGHUP
  LOGDEBUG("Registering handler for runtime configuration update...");
  signal(SIGHUP, UpdateHelper);

  return ;
}
